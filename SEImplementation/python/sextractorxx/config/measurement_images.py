from __future__ import division, print_function

import os
import re
from astropy.io import fits

import libSEImplementation as cpp

measurement_images = {}


class MeasurementImage(cpp.MeasurementImage):

    def __init__(self, fits_file, psf_file=None, weight_file=None, gain=None,
                 gain_keyword='GAIN', saturation=None, saturation_keyword='SATURATE',
                 flux_scale=None, flux_scale_keyword='FLXSCALE',
                 weight_type='background', weight_absolute=False, weight_scaling=1.,
                 weight_threshold=None):
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
        return 'Image {}: {}, PSF: {}, Weight: {}'.format(
            self.id, self.meta['IMAGE_FILENAME'], self.meta['PSF_FILENAME'],
            self.meta['WEIGHT_FILENAME'])


def print_measurement_images():
    print('Measurement images:')
    for i in measurement_images:
        im = measurement_images[i]
        print('Image {}'.format(i))
        print('      File: {}'.format(im.file))
        print('       PSF: {}'.format(im.psf_file))
        print('    Weight: {}'.format(im.weight_file))


class ImageGroup(object):

    def __init__(self, **kwargs):
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
            print(self.__subgroups)
            for name, _ in self.__subgroups:
                self.__subgroup_names.add(name)

    def __len__(self):
        if self.__subgroups:
            return len(self.__subgroups)
        else:
            return len(self.__images)

    def __iter__(self):
        if self.__subgroups:
            return self.__subgroups.__iter__()
        else:
            return self.__images.__iter__()

    def split(self, grouping_method):
        if self.__subgroups:
            raise Exception('ImageGroup is already subgrouped')
        subgrouped_images = grouping_method(self.__images)
        if sum(len(p[1]) for p in subgrouped_images) != len(self.__images):
            self.__subgroups = None
            raise Exception('Some images were not grouped')
        self.__subgroups = []
        for k, im_list in subgrouped_images:
            assert k not in self.__subgroup_names
            self.__subgroup_names.add(k)
            self.__subgroups.append((k, ImageGroup(images=im_list)))
        self.__images = []

    def add_images(self, images):
        if self.__subgroups is not None:
            raise Exception('ImageGroup is already subgrouped')
        if isinstance(images, MeasurementImage):
            self.__images.append(images)
        else:
            self.__images.extend(images)

    def add_subgroup(self, name, group):
        if self.__subgroups is None:
            raise Exception('ImageGroup is not subgrouped yet')
        if name in self.__subgroup_names:
            raise Exception('Subgroup {} alread exists'.format(name))
        self.__subgroup_names.add(name)
        self.__subgroups.append((name, group))

    def is_leaf(self):
        return self.__subgroups is None

    def __getitem__(self, name):
        if self.__subgroups is None:
            raise Exception('ImageGroup is not subgrouped yet')
        return (x for x in self.__subgroups if x[0] == name).next()[1]

    def printToScreen(self, prefix='', show_images=False):
        if self.__subgroups is None:
            print('{}Image List ({})'.format(prefix, len(self.__images)))
            if show_images:
                for im in self.__images:
                    print('{}{}'.format(prefix, im))
        else:
            print('{}Sub-groups: {}'.format(prefix, ','.join(x for x, _ in self.__subgroups)))
            for name, group in self.__subgroups:
                print('{}  {}:'.format(prefix, name))
                group.printToScreen(prefix + '    ', show_images)


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

    :param im: Relative path to the image FITS file
    :param kwargs:
        These will be forwarded to the constructor, or used to verify the parameters match the previous instantiation
    :raise:
        If `im` was already loaded before, but the parameters do not match what was used before.
    :return:
        A MeasurementImage instance
    """
    if im in _image_cache:
        entry = _image_cache[im]
        mismatch = entry.match_kwargs(kwargs)
        if mismatch:
            raise Exception(
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

    :param image_list: A list of relative paths to the images FITS files
    :param psf_list: A list of relative paths to the PSF FITS files
    :param weight_list: A list of relative paths to the weight files (optional)
    :return: A ImageGroup representing the images
    """
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

    def __init__(self, key):
        self.__key = key

    def __call__(self, images):
        result = {}
        for im in images:
            assert self.__key in im.meta
            if im.meta[self.__key] not in result:
                result[im.meta[self.__key]] = []
            result[im.meta[self.__key]].append(im)
        return [(k, result[k]) for k in result]


class ByPattern(object):

    def __init__(self, key, pattern):
        self.__key = key
        self.__pattern = pattern

    def __call__(self, images):
        result = {}
        for im in images:
            assert self.__key in im.meta
            group = re.match(self.__pattern, im.meta[self.__key]).group(1)
            if group not in result:
                result[group] = []
            result[group].append(im)
        return [(k, result[k]) for k in result]


class MeasurementGroup(object):

    def __init__(self, image_group):
        self.__images = None
        self.__subgroups = None
        if image_group.is_leaf():
            self.__images = [im for im in image_group]
        else:
            self.__subgroups = [(n, MeasurementGroup(g)) for n,g in image_group]
        self.__models = []

    def __iter__(self):
        if self.__subgroups:
            return self.__subgroups.__iter__()
        else:
            return self.__images.__iter__()

    def __getitem__(self, name):
        if self.__subgroups is None:
            raise Exception('Does not contain subgroups')
        return (x for x in self.__subgroups if x[0] == name).next()[1]

    def __len__(self):
        if self.__subgroups:
            return len(self.__subgroups)
        else:
            return len(self.__images)

    def is_leaf(self):
        return self.__subgroups is None

    def printToScreen(self, prefix='', show_images=False, show_params=False):
        if self.__images:
            print('{}Image List ({})'.format(prefix, len(self.__images)))
            if show_images:
                for im in self.__images:
                    print('{}{}'.format(prefix, im))
        if self.__subgroups:
            print('{}Sub-groups: {}'.format(prefix, ','.join(
                x for x, _ in self.__subgroups)))
            for name, group in self.__subgroups:
                print('{}  {}:'.format(prefix, name))
                group.printToScreen(prefix + '    ', show_images, show_params)
