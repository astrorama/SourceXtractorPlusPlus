from __future__ import division, print_function

import os
import re
from astropy.io import fits


class MeasurementImage(object):

    def __init__(self, fits_file, hdu, cube_index, psf_file, psf_hdu):
        self.meta = {
            'IMAGE_FILENAME' : os.path.abspath(fits_file),
            'IMAGE_HDU_NO' : hdu,
            'IMAGE_CUBE_INDEX' : cube_index,
            'PSF_FILENAME' : os.path.abspath(psf_file),
            'PSF_HDU_NO' : psf_hdu
        }
        hdu_list = fits.open(fits_file)
        hdu_meta = hdu_list[hdu].header
        for key in hdu_meta:
            self.meta[key] = hdu_meta[key]

    def __str__(self):
        return 'Image: {}, HDU: {}, Cube: {}, PSF: {}, PSF HDU: {}'.format(
            self.meta['IMAGE_FILENAME'], self.meta['IMAGE_HDU_NO'],
            self.meta['IMAGE_CUBE_INDEX'], self.meta['PSF_FILENAME'],
            self.meta['PSF_HDU_NO'])


class MeasurementGroup(object):

    def __init__(self, **kwargs):
        self.__images = []
        self.__subgroups = None
        self.__subgroup_names = set()
        assert len(kwargs) == 1
        key = kwargs.keys()[0]
        if key == 'images':
            if isinstance(kwargs[key], list):
                self.__images = kwargs[key]
            else:
                self.__images = [kwargs[key]]
        if key == 'subgroups':
            self.__subgroups = kwargs[key]
            for name, _ in self._subgroups:
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
            raise Exception('MeasurementGroup is already subgrouped')
        subgrouped_images = grouping_method(self.__images)
        if sum(len(p[1]) for p in subgrouped_images) != len(self.__images):
            self.__subgroups = None
            raise Exception ('Some images were not grouped')
        self.__subgroups = []
        for k, im_list in subgrouped_images:
            assert k not in self.__subgroup_names
            self.__subgroup_names.add(k)
            self.__subgroups.append((k, MeasurementGroup(images=im_list)))
        self.__images = []

    def add_images(self, images):
        if self.__subgroups is not None:
            raise Exception('MeasurementGroup is already subgrouped')
        if isinstance(images, MeasurementImage):
            self.__images.append(images)
        else:
            self.__images.extend(images)

    def add_subgroup(self, name, group):
        if self.__subgroups is None:
            raise Exception('MeasurementGroup is not subgrouped yet')
        if name in self.__subgroup_names:
            raise Exception('Subgroup {} alread exists'.format(name))
        self.__subgroup_names.add(name)
        self.__subgroups.append((name, group))

    def is_leaf(self):
        return self.__subgroups is None

    def __getitem__(self, name):
        if self.__subgroups is None:
            raise Exception('MeasurementGroup is not subgrouped yet')
        return (x for x in self.__subgroups if x[0] == name).next()[1]

    def printToScreen(self, prefix='', show_images=False):
        if self.__subgroups is None:
            print('{}Image List ({})'.format(prefix, len(self.__images)))
            if show_images:
                for im in self.__images:
                    print('{}{}'.format(prefix, im))
        else:
            print('{}Sub-groups: {}'.format(prefix, ','.join(x for x,_ in self.__subgroups)))
            for name, group in self.__subgroups:
                print('{}  {}:'.format(prefix, name))
                group.printToScreen(prefix + '    ', show_images)


def load_fits_images(image_list, psf_list, hdu_list=None, psf_hdu_list=None):
    """Creates an image group for the given images.

    The parameter images is a list of relative paths to the FITS files containing
    the images. The optional parameter hdu_list can be used to control the HDU of
    each file where the image is stored. By default the primary HDU is used. The
    psf_list must contain relative paths to the FITS files containing the PSF of
    each image and it must match one-to-one the images in the image_list. The
    optional parameter psf_hdu_list must contain the HDU the PSF is in each file.
    By default the same HDU numbers of the image FITS are used.

    :param image_list: A list of relative paths to the images FITS files
    :param psf_list: A list of relative paths to the PSF FITS files
    :param hdu_list: A list of the HDU numbers where each image is stored (optional)
    :param psf_hdu_list: A list of the HDU numbers where the PSFs are stored (optional)
    :return: A MeasurementGroup representing the images
    """
    assert len(image_list) == len(psf_list)
    if hdu_list is None:
        hdu_list = [0] * len(image_list)
    else:
        assert len(image_list) == len(hdu_list)
    if psf_hdu_list is None:
        psf_hdu_list = hdu_list
    else:
        assert len(psf_hdu_list) == len(psf_list)
    meas_image_list = []
    for im, hdu, psf, psf_hdu in zip (image_list, hdu_list, psf_list, psf_hdu_list):
        meas_image_list.append(MeasurementImage(im, hdu, 0, psf, psf_hdu))
    return MeasurementGroup(images=meas_image_list)


def load_multi_hdu_fits(image_file, psf):
    """Creates an image group with the images of a multi-HDU FITS file.

    The psf parameter can either be a multi-HDU FITS file where the HDUs match
    one-to-one the HDUs of the image file or a list of single HDU PSFs. Note that
    any HDUs not containing images (two dimensional arrays) are ignored.

    :param image_file: The multi-HDU FITS file containing the images
    :param psf: Either a multi-HDU FITS fie containing the PSFs or a list of
        single HDU FITS files, one for each PSF
    :return: A MeasurementGroup representing the images
    """
    hdu_list = [i for i, hdu in enumerate(fits.open(image_file)) if hdu.is_image and hdu.header['NAXIS'] == 2]
    if isinstance(psf, list):
        assert len(psf) == len(hdu_list)
        psf_list = psf
        psf_hdu_list = [0] * len(psf_list)
    else:
        psf_list = [psf] * len(hdu_list)
        psf_hdu_list = hdu_list
    meas_image_list = []
    for hdu, psf, psf_hdu in zip(du_list, psf_list, psf_hdu_list):
        meas_image_list.append(MeasurementImage(image_file, hdu, 0, psf, psf_hdu))
    return MeasurementGroup(images=meas_image_list)


def load_fits_cube(image_file, psf, hdu=0):
    """Creates an image group with the immages of a FITS cube HDU.

    The psf parameter can either be a multi-HDU FITS file with as many HDUs as the
    cubes third dimension or a list of single HDU PSFs. Note that all the images
    of the cube will share in their metadata the information of the FITS header.

    :param image_file: The cube FITS file
    :param psf: Either a multi-HDU FITS fie containing the PSFs or a list of
        single HDU FITS files, one for each PSF
    :param hdu: The HDU of the image_file containing the cube (optional)
    :return: A MeasurementGroup representing the images
    """
    fits_header = fits.open(image_file)[hdu].header
    assert fits_header['NAXIS'] == 3
    image_no = fits_header['NAXIS3']
    if isinstance(psf, list):
        assert len(psf) == image_no
        psf_list = psf
        psf_hdu_list = [0] * image_no
    else:
        psf_list = [psf] * image_no
        psf_hdu_list = range(image_no)
    meas_image_list = []
    for i, psf, psf_hdu in zip(range(image_no), psf_list, psf_hdu_list):
        meas_image_list.append(MeasurementImage(image_file, hdu, i, psf, psf_hdu))
    return MeasurementGroup(images=meas_image_list)


class GroupByKeyword(object):

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


class GroupByPattern(object):

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
