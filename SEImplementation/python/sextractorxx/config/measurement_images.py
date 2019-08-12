from __future__ import division, print_function

import os
import re
import sys

from astropy.io import fits

import _SExtractorPy as cpp

if sys.version_info.major < 3:
    from StringIO import StringIO
else:
    from io import StringIO


measurement_images = {}


class MeasurementImage(cpp.MeasurementImage):
    """
    A MeasurementImage is the processing unit for SExtractor++. Measurements and model fitting can be done
    over one, or many, of them. It models the image, plus its associated weight file, PSF, etc.

    Parameters
    ----------
    fits_file : str
        The path to a FITS image. Only primary HDU images are supported.
    psf_file : str
        The path to a PSF. It can be either a FITS image, or a PSFEx model.
    weight_file : str
        The path to a FITS image with the pixel weights.
    gain : float
        Image gain. If None, `gain_keyword` will be used instead.
    gain_keyword : str
        Keyword for the header containing the gain.
    saturation : float
        Saturation value. If None, `saturation_keyword` will be used instead.
    saturation_keyword : str
        Keyword for the header containing the saturation value.
    flux_scale : float
        Flux scaling. Each pixel value will be multiplied by this. If None, `flux_scale_keyword` will be used
        instead.
    flux_scale_keyword : str
        Keyword for the header containing the flux scaling.
    weight_type : str
        The type of the weight image. It must be one of:

            - background
                The image itself is used to compute internally a variance map (default)
            - rms
                The weight image must contain a weight-map in units of absolute standard deviations
                (in ADUs per pixel).
            - variance
                The weight image must contain a weight-map in units of relative variance.
            - weight
                The weight image must contain a weight-map in units of relative weights. The data are converted
                to variance units.
    weight_absolute : bool
        If False, the weight map will be scaled according to an absolute variance map built from the image itself.
    weight_scaling : float
        Apply an scaling to the weight map.
    weight_threshold : float
        Pixels with weights beyond this value are treated just like pixels discarded by the masking process.
    """

    def __init__(self, fits_file, psf_file=None, weight_file=None, gain=None,
                 gain_keyword='GAIN', saturation=None, saturation_keyword='SATURATE',
                 flux_scale=None, flux_scale_keyword='FLXSCALE',
                 weight_type='background', weight_absolute=False, weight_scaling=1.,
                 weight_threshold=None):
        """
        Constructor.
        """
        super(MeasurementImage, self).__init__(os.path.abspath(fits_file),
                                               os.path.abspath(psf_file) if psf_file else '',
                                               os.path.abspath(weight_file) if weight_file else '')

        self.meta = {
            'IMAGE_FILENAME': self.file,
            'PSF_FILENAME': self.psf_file,
            'WEIGHT_FILENAME': self.weight_file
        }
        hdu_list = fits.open(fits_file)
        hdu_meta = hdu_list[0].header
        for key in hdu_meta:
            self.meta[key] = hdu_meta[key]

        if gain is not None:
            self.gain = gain
        elif gain_keyword in self.meta:
            self.gain = float(self.meta[gain_keyword])
        else:
            self.gain = 0.

        if saturation is not None:
            self.saturation = saturation
        elif saturation_keyword in self.meta:
            self.saturation = float(self.meta[saturation_keyword])
        else:
            self.saturation = 0.

        if flux_scale is not None:
            self.flux_scale = flux_scale
        elif flux_scale_keyword in self.meta:
            self.flux_scale = float(self.meta[flux_scale_keyword])
        else:
            self.flux_scale = 1.

        self.weight_type = weight_type
        self.weight_absolute = weight_absolute
        self.weight_scaling = weight_scaling
        if weight_threshold is None:
            self.has_weight_threshold = False
        else:
            self.has_weight_threshold = True
            self.weight_threshold = weight_threshold

        global measurement_images
        measurement_images[self.id] = self

    def __str__(self):
        """
        Returns
        -------
        str
            Human readable representation for the object
        """
        return 'Image {}: {}, PSF: {}, Weight: {}'.format(
            self.id, self.meta['IMAGE_FILENAME'], self.meta['PSF_FILENAME'],
            self.meta['WEIGHT_FILENAME'])


def print_measurement_images(file=sys.stderr):
    """
    Print a human-readable representation of the configured measurement images.

    Parameters
    ----------
    file : file object
        Where to print the representation. Defaults to sys.stderr
    """
    print('Measurement images:', file=file)
    for i in measurement_images:
        im = measurement_images[i]
        print('Image {}'.format(i), file=file)
        print('      File: {}'.format(im.file), file=file)
        print('       PSF: {}'.format(im.psf_file), file=file)
        print('    Weight: {}'.format(im.weight_file), file=file)


class ImageGroup(object):
    """
    Models the grouping of images. Measurement can *not* be made directly on instances of this type.
    The configuration must be "frozen" before creating a MeasurementGroup

    See Also
    --------
    MeasurementGroup
    """

    def __init__(self, **kwargs):
        """
        Constructor. It is not recommended to be used directly. Use instead load_fits_image or load_fits_images.
        """
        self.__images = []
        self.__subgroups = None
        self.__subgroup_names = set()
        assert len(kwargs) == 1
        key = list(kwargs.keys())[0]
        if key == 'images':
            if isinstance(kwargs[key], list):
                self.__images = kwargs[key]
            else:
                self.__images = [kwargs[key]]
        if key == 'subgroups':
            self.__subgroups = kwargs[key]
            for name, _ in self.__subgroups:
                self.__subgroup_names.add(name)

    def __len__(self):
        """
        See Also
        --------
        is_leaf

        Returns
        -------
        int
            How may subgroups or images are there in this group
        """
        if self.__subgroups:
            return len(self.__subgroups)
        else:
            return len(self.__images)

    def __iter__(self):
        """
        Allows to iterate on the contained subgroups or images

        See Also
        --------
        is_leaf

        Returns
        -------
        iterator
        """
        if self.__subgroups:
            return self.__subgroups.__iter__()
        else:
            return self.__images.__iter__()

    def split(self, grouping_method):
        """
        Splits the group in various subgroups, applying a filter on the contained images.

        Parameters
        ----------
        grouping_method : callable
            A callable that receives as a parameter the list of contained images, and returns
            a list of tuples, with the grouping key value, and the list of grouped images belonging to the given key.

        See Also
        --------
        ByKeyword
        ByPattern

        Raises
        -------
        ValueError
            If the group has been already split, or if some images have not been grouped by the callable.
        """
        if self.__subgroups:
            raise ValueError('ImageGroup is already subgrouped')
        subgrouped_images = grouping_method(self.__images)
        if sum(len(p[1]) for p in subgrouped_images) != len(self.__images):
            self.__subgroups = None
            raise ValueError('Some images were not grouped')
        self.__subgroups = []
        for k, im_list in subgrouped_images:
            assert k not in self.__subgroup_names
            self.__subgroup_names.add(k)
            self.__subgroups.append((k, ImageGroup(images=im_list)))
        self.__images = []

    def add_images(self, images):
        """
        Add new images to the group.

        Parameters
        ----------
        images : list of, or a single, MeasurementImage

        Raises
        ------
        ValueError
            If the group has been split, no new images can be added.
        """
        if self.__subgroups is not None:
            raise ValueError('ImageGroup is already subgrouped')
        if isinstance(images, MeasurementImage):
            self.__images.append(images)
        else:
            self.__images.extend(images)

    def add_subgroup(self, name, group):
        """
        Add a subgroup to a group.

        Parameters
        ----------
        name : str
            The new of the new group

        group : ImageGroup
        """
        if self.__subgroups is None:
            raise Exception('ImageGroup is not subgrouped yet')
        if name in self.__subgroup_names:
            raise Exception('Subgroup {} alread exists'.format(name))
        self.__subgroup_names.add(name)
        self.__subgroups.append((name, group))

    def is_leaf(self):
        """
        Returns
        -------
        bool
            True if the group is a leaf group
        """
        return self.__subgroups is None

    def __getitem__(self, name):
        """
        Get a subgroup.

        Parameters
        ----------
        name : str
            The name of the subgroup.

        Returns
        -------
        ImageGroup
            The matching group.

        Raises
        ------
        ValueError
            If the group has not been split.
        KeyError
            If the group has not been found.
        """
        if self.__subgroups is None:
            raise ValueError('ImageGroup is not subgrouped yet')
        try:
            return next(x for x in self.__subgroups if x[0] == name)[1]
        except StopIteration:
            raise KeyError('Group {} not found'.format(name))

    def print(self, prefix='', show_images=False, file=sys.stderr):
        """
        Print a human-readable representation of the group.

        Parameters
        ----------
        prefix : str
            Print each line with this prefix. Used internally for indentation.
        show_images : bool
            Show the images belonging to a leaf group.
        file : file object
            Where to print the representation. Defaults to sys.stderr
        """
        if self.__subgroups is None:
            print('{}Image List ({})'.format(prefix, len(self.__images)), file=file)
            if show_images:
                for im in self.__images:
                    print('{}{}'.format(prefix, im), file=file)
        else:
            print('{}Sub-groups: {}'.format(prefix, ','.join(str(x) for x, _ in self.__subgroups)), file=file)
            for name, group in self.__subgroups:
                print('{}  {}:'.format(prefix, name), file=file)
                group.print(prefix + '    ', show_images, file)

    def __str__(self):
        """
        Returns
        -------
        str
            A human-readable representation of the group
        """
        string = StringIO()
        self.print(show_images=True, file=string)
        return string.getvalue()


class ImageCacheEntry(object):
    def __init__(self, image, kwargs):
        self.image = image
        self.kwargs = kwargs

    def match_kwargs(self, kwargs):
        mismatch = []
        for key, value in kwargs.items():
            if key not in self.kwargs:
                mismatch.append('{} {} != undefined'.format(key, value))
            elif self.kwargs[key] != value:
                mismatch.append('{} {} != {}'.format(key, value, self.kwargs[key]))
        return mismatch

_image_cache = {}


def load_fits_image(im, **kwargs):
    """Returns a new MeasurementImage, or an existing one if the image was already loaded.

    If the image was loaded previously, then the arguments will be cross-checked to make sure it was instantiated
    either with the same parameters, or at least with a superset (i.e on the second call you may skip a parameter,
    but you may *not* add a new one).

    Parameters
    ----------
    im : str
        Relative path to the image FITS file
    kwargs :
        These will be forwarded to the constructor, or used to verify the parameters match the previous instantiation

    Raises
    ------
    ValueError
        If `im` was already loaded before, but the parameters do not match what was used before.

    Returns
    -------
    MeasurementImage
        A MeasurementImage instance
    """
    if im in _image_cache:
        entry = _image_cache[im]
        mismatch = entry.match_kwargs(kwargs)
        if mismatch:
            raise ValueError(
                'The image "{}" was constructed before with different parameters: {}'.format(im, ', '.join(mismatch))
            )
        return entry.image
    _image_cache[im] = ImageCacheEntry(MeasurementImage(im, **kwargs), kwargs)
    return _image_cache[im].image


def load_fits_images(image_list, psf_list=None, weight_list=None):
    """Creates an image group for the given images.

    The parameter images is a list of relative paths to the FITS files containing
    the images. The optional parameter hdu_list can be used to control the HDU of
    each file where the image is stored. By default the primary HDU is used. The
    psf_list must contain relative paths to the FITS files containing the PSF of
    each image and it must match one-to-one the images in the image_list. Finally,
    the weight_list can be used to define the weight images. The entries of this
    list can be either a filename or one of None and empty string, which mean no
    weights.

    Note that this method will set the gain, saturation level and fux scale using
    the header keywords GAIN, SATURATE and FLUXSCALE respectively, or the values
    0., 0., and 1. in the cases the keywords are missing. To override this
    default behavior one should create the ImageGroup manually from a list of
    MeasurementImage instances.

    The same is true for the weight map configuration, where the type is set to
    background, the scaling to 1, the weights are treated as relative and there
    is no weight threshold.

    Parameters
    ----------
    image_list : list of str
        A list of relative paths to the images FITS files.
    psf_list : list of str
        A list of relative paths to the PSF FITS files (optional). It must match the length of image_list or be None.
    weight_list : list of str
        A list of relative paths to the weight files (optional). It must match the length of image_list or be None.

    Returns
    -------
    ImageGroup
        A ImageGroup representing the images

    Raises
    ------
    ValueError
        The image list if empty
    AssertionError
        If psf_list or weight_list are not None, and their length do not match the length of image_list
    """
    if len(image_list) == 0:
        raise ValueError('An empty list passed to load_fits_images')
    if psf_list is None:
        psf_list = [None] * len(image_list)
    else:
        assert len(image_list) == len(psf_list)
    if weight_list is None:
        weight_list = [None] * len(image_list)
    else:
        assert len(image_list) == len(weight_list)
    meas_image_list = []
    for im, psf, w in zip(image_list, psf_list, weight_list):
        meas_image_list.append(
            load_fits_image(im, psf_file=psf, weight_file=w)
        )
    return ImageGroup(images=meas_image_list)


# def load_multi_hdu_fits(image_file, psf):
#     """Creates an image group with the images of a multi-HDU FITS file.
#
#     The psf parameter can either be a multi-HDU FITS file where the HDUs match
#     one-to-one the HDUs of the image file or a list of single HDU PSFs. Note that
#     any HDUs not containing images (two dimensional arrays) are ignored.
#
#     :param image_file: The multi-HDU FITS file containing the images
#     :param psf: Either a multi-HDU FITS fie containing the PSFs or a list of
#         single HDU FITS files, one for each PSF
#     :return: A ImageGroup representing the images
#     """
#     hdu_list = [i for i, hdu in enumerate(fits.open(image_file)) if hdu.is_image and hdu.header['NAXIS'] == 2]
#     if isinstance(psf, list):
#         assert len(psf) == len(hdu_list)
#         psf_list = psf
#         psf_hdu_list = [0] * len(psf_list)
#     else:
#         psf_list = [psf] * len(hdu_list)
#         psf_hdu_list = hdu_list
#     meas_image_list = []
#     for hdu, psf, psf_hdu in zip(du_list, psf_list, psf_hdu_list):
#         meas_image_list.append(MeasurementImage(image_file, hdu, 0, psf, psf_hdu))
#     return ImageGroup(images=meas_image_list)
#
#
# def load_fits_cube(image_file, psf, hdu=0):
#     """Creates an image group with the immages of a FITS cube HDU.
#
#     The psf parameter can either be a multi-HDU FITS file with as many HDUs as the
#     cubes third dimension or a list of single HDU PSFs. Note that all the images
#     of the cube will share in their metadata the information of the FITS header.
#
#     :param image_file: The cube FITS file
#     :param psf: Either a multi-HDU FITS fie containing the PSFs or a list of
#         single HDU FITS files, one for each PSF
#     :param hdu: The HDU of the image_file containing the cube (optional)
#     :return: A ImageGroup representing the images
#     """
#     fits_header = fits.open(image_file)[hdu].header
#     assert fits_header['NAXIS'] == 3
#     image_no = fits_header['NAXIS3']
#     if isinstance(psf, list):
#         assert len(psf) == image_no
#         psf_list = psf
#         psf_hdu_list = [0] * image_no
#     else:
#         psf_list = [psf] * image_no
#         psf_hdu_list = range(image_no)
#     meas_image_list = []
#     for i, psf, psf_hdu in zip(range(image_no), psf_list, psf_hdu_list):
#         meas_image_list.append(MeasurementImage(image_file, hdu, i, psf, psf_hdu))
#     return ImageGroup(images=meas_image_list)


class ByKeyword(object):
    """
    Callable that can be used to split an ImageGroup by a keyword value (i.e. FILTER).

    Parameters
    ----------
    key : str
        FITS header keyword (i.e. FILTER)

    See Also
    --------
    ImageGroup.split
    """

    def __init__(self, key):
        """
        Constructor.
        """
        self.__key = key

    def __call__(self, images):
        """
        Parameters
        ----------
        images : list of MeasurementImage
            List of images to group

        Returns
        -------
        list of tuples of str and list of MeasurementImage
            i.e. [
                (R, [frame_r_01.fits,  frame_r_02.fits]),
                (G, [frame_g_01.fits,  frame_g_02.fits])
            ]
        """
        result = {}
        for im in images:
            assert self.__key in im.meta
            if im.meta[self.__key] not in result:
                result[im.meta[self.__key]] = []
            result[im.meta[self.__key]].append(im)
        return [(k, result[k]) for k in result]


class ByPattern(object):
    """
    Callable that can be used to split an ImageGroup by a keyword value (i.e. FILTER), applying a regular
    expression and using the first matching group as key.

    Parameters
    ----------
    key : str
        FITS header keyword
    pattern : str
        Regular expression. The first matching group will be used as grouping key.

    See Also
    --------
    ImageGroup.split
    """

    def __init__(self, key, pattern):
        """
        Constructor.
        """
        self.__key = key
        self.__pattern = pattern

    def __call__(self, images):
        """
        Parameters
        ----------
        images : list of MeasurementImage
            List of images to group

        Returns
        -------
        list of tuples of str and list of MeasurementImage
        """
        result = {}
        for im in images:
            assert self.__key in im.meta
            group = re.match(self.__pattern, im.meta[self.__key]).group(1)
            if group not in result:
                result[group] = []
            result[group].append(im)
        return [(k, result[k]) for k in result]


class MeasurementGroup(object):
    """
    Once an instance of this class is created from an ImageGroup, its configuration is "frozen". i.e.
    no new images can be added, or no new grouping applied.

    Parameters
    ----------
    image_group : ImageGroup
    """

    def __init__(self, image_group):
        """
        Constructor.
        """
        self.__images = None
        self.__subgroups = None
        if image_group.is_leaf():
            self.__images = [im for im in image_group]
        else:
            self.__subgroups = [(n, MeasurementGroup(g)) for n,g in image_group]

    def __iter__(self):
        """
        Returns
        -------
        iterator
        """
        if self.__subgroups:
            return self.__subgroups.__iter__()
        else:
            return self.__images.__iter__()

    def __getitem__(self, name):
        """
        The subgroup with the given name.

        Parameters
        ----------
        name : str
            Subgroup name

        Returns
        -------
        MeasurementGroup

        Raises
        ------
        ValueError
            If the group does not have subgroups.
        KeyError
            If the group has not been found.
        """
        if self.__subgroups is None:
            raise ValueError('Does not contain subgroups')
        try:
            return next(x for x in self.__subgroups if x[0] == name)[1]
        except StopIteration:
            raise KeyError('Group {} not found'.format(name))

    def __len__(self):
        """
        Returns
        -------
        int
            Number of subgroups, or images contained within the group
        """
        if self.__subgroups:
            return len(self.__subgroups)
        else:
            return len(self.__images)

    def is_leaf(self):
        """
        Returns
        -------
        bool
            True if the group is a leaf group
        """
        return self.__subgroups is None

    def print(self, prefix='', show_images=False, file=sys.stderr):
        """
        Print a human-readable representation of the group.

        Parameters
        ----------
        prefix : str
            Print each line with this prefix. Used internally for indentation.
        show_images : bool
            Show the images belonging to a leaf group.
        file : file object
            Where to print the representation. Defaults to sys.stderr
        """
        if self.__images:
            print('{}Image List ({})'.format(prefix, len(self.__images)), file=file)
            if show_images:
                for im in self.__images:
                    print('{}{}'.format(prefix, im), file=file)
        if self.__subgroups:
            print('{}Sub-groups: {}'.format(prefix, ','.join(
                x for x, _ in self.__subgroups)), file=file)
            for name, group in self.__subgroups:
                print('{}  {}:'.format(prefix, name), file=file)
                group.print(prefix + '    ', show_images, file=file)

    def __str__(self):
        """
        Returns
        -------
        str
            A human-readable representation of the group
        """
        string = StringIO()
        self.print(show_images=True, file=string)
        return string.getvalue()
