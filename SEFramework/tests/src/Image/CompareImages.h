/**
 * @file CompareImages.h
 * @date 11/09/18
 * @author Alejandro Alvarez Ayllon
 */


#ifndef _COMPAREIMAGES_H
#define _COMPAREIMAGES_H

namespace SExtractor {

boost::test_tools::predicate_result compareImages(
  const std::shared_ptr<Image<SeFloat>> &ref, const std::shared_ptr<Image<SeFloat>> &padded) {
  boost::test_tools::predicate_result res(true);

  for (int x = 0; x < ref->getWidth(); ++x) {
    for (int y = 0; y < ref->getHeight(); ++y) {
      auto expected = ref->getValue(x, y);
      auto value = padded->getValue(x, y);
      if (expected != value) {
        res = false;
        res.message() << "Not matching values at position " << x << "," << y
                      << ": " << expected << " != " << value << "\n";
      }
    }
  }

  return res;
}

} // end SExtractor

#endif // _COMPAREIMAGES_H
