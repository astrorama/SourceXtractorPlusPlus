import os

import pytest

from sourcextractor.config import measurement_images as mi


def auxiliary(name: str) -> str:
    """Locate a test fixture the way ElementsKernel's getAuxiliaryPath does."""
    for root in os.environ.get('ELEMENTS_AUX_PATH', '').split(os.pathsep):
        candidate = os.path.join(root, name)
        if os.path.exists(candidate):
            return candidate
    pytest.skip(f'fixture {name} not on ELEMENTS_AUX_PATH')


@pytest.fixture
def fits_with_primary():
    return auxiliary('with_primary.fits')


@pytest.fixture
def fits_metadata(fits_with_primary):
    return mi.cpp.ImageFile(fits_with_primary).get_metadata(0)


@pytest.fixture
def asdf_metadata():
    return mi.cpp.ImageFile(auxiliary('metadata.asdf')).get_metadata('data')
