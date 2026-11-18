"""
Reading image metadata from a configuration script.

Covers the format-agnostic path--cpp.ImageFile and the Metadata handle it
returns--for both FITS and ASDF.  Fixtures come from SEFramework's auxdir,
which is where the equivalent C++ tests get theirs.
"""

import pytest

from sourcextractor.config import measurement_images as mi


class TestFitsMetadata:
    def test_keyword_lookup(self, fits_metadata):
        assert fits_metadata['GAIN'] == 42.0
        assert fits_metadata['STRF'] == "A string 'header'"

    def test_values_keep_their_types(self, fits_metadata):
        """Unlike get_headers(), which stringifies everything"""
        assert isinstance(fits_metadata['GAIN'], float)
        assert isinstance(fits_metadata['NAXIS'], int)
        assert isinstance(fits_metadata['STRF'], str)

    def test_logical_keyword_is_a_one_character_string(self, fits_metadata):
        """
        FitsFile parses an unquoted single character as a char, so a FITS
        logical arrives as 'T' or 'F' rather than a bool
        """
        assert fits_metadata['SIMPLE'] == 'T'

    def test_contains(self, fits_metadata):
        assert 'GAIN' in fits_metadata
        assert 'NOSUCHKEY' not in fits_metadata

    def test_missing_key_raises_keyerror(self, fits_metadata):
        with pytest.raises(KeyError):
            fits_metadata['NOSUCHKEY']

    def test_get_with_default(self, fits_metadata):
        assert fits_metadata.get('GAIN') == 42.0
        assert fits_metadata.get('NOSUCHKEY') is None
        assert fits_metadata.get('NOSUCHKEY', 'fallback') == 'fallback'


class TestFitsFileIsAnImageFile:
    def test_inherits_image_file(self, fits_file):
        assert isinstance(fits_file, mi.cpp.ImageFile)

    def test_inherits_filename(self, fits_file, fits_with_primary):
        assert fits_file.filename == fits_with_primary

    def test_inherits_get_metadata(self, fits_file):
        assert fits_file.get_metadata(0)['GAIN'] == 42.0

    def test_get_headers_still_stringifies(self, fits_file):
        """Kept as it was; get_metadata is the type-preserving counterpart"""
        assert fits_file.get_headers(0)['GAIN'] == '42'

    def test_an_int_means_the_same_hdu_either_way(self, fits_file):
        """
        get_metadata(n) is overridden for FITS so that n is the absolute HDU,
        as it is for get_headers(n) and get_dimensions(n)
        """
        headers = fits_file.get_headers(0)
        metadata = fits_file.get_metadata(0)
        keys = [entry.key() if callable(entry.key) else entry.key
                for entry in headers]
        assert keys
        for key in keys:
            assert key in metadata


class TestAsdfMetadata:
    def test_lookup_by_tree_path(self, asdf_metadata):
        """
        Keys are the slash-separated paths libasdf reports, which is also
        what the --*-image-name options take
        """
        assert asdf_metadata['/meta/instrument/optical_element'] == 'F158'
        assert asdf_metadata['/meta/gain'] == 2.5

    def test_values_keep_their_types(self, asdf_metadata):
        assert isinstance(asdf_metadata['/meta/gain'], float)
        assert isinstance(asdf_metadata['/meta/instrument/optical_element'], str)

    def test_leading_slash_is_optional(self, asdf_metadata):
        assert asdf_metadata['meta/gain'] == asdf_metadata['/meta/gain']

    def test_sequence_elements_are_indexed(self, asdf_metadata):
        assert asdf_metadata['/meta/filters/0'] == 'F158'
        assert asdf_metadata['/meta/filters/1'] == 'F184'

    def test_ndarray_nodes_are_walked(self, asdf_metadata):
        """An ndarray is a mapping too, and may carry keys of its own"""
        assert asdf_metadata['/data/datatype'] == 'float32'

    def test_containers_have_no_value(self, asdf_metadata):
        """
        A mapping or sequence is descended into, never a value itself

        This is a decision made for now for simplicity of compatibility with
        the FITS metadata model, where only scalar values are considered.
        Might want to revisit this decision later especially if ASDF is used
        more.
        """
        for key in ('/meta', '/meta/instrument', '/meta/filters', '/data'):
            assert key not in asdf_metadata
            with pytest.raises(KeyError):
                asdf_metadata[key]

    def test_missing_key(self, asdf_metadata):
        assert '/meta/nope' not in asdf_metadata
        assert asdf_metadata.get('/meta/nope', 'fallback') == 'fallback'


def test_fits_and_asdf_share_one_metadata_type(fits_metadata, asdf_metadata):
    """One metadata type regardless of file format"""
    assert type(fits_metadata) is type(asdf_metadata)
