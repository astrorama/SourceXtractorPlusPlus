/*
 * @file SEFramework/Convolution/Convolution.h
 * @date 17/09/18
 * @author aalvarez
 */

#ifndef _SEFRAMEWORK_CONVOLUTION_CONVOLUTION_H
#define _SEFRAMEWORK_CONVOLUTION_CONVOLUTION_H

#include "DirectConvolution.h"
#include "DFT.h"

namespace SExtractor {

/**
 * For convenience, default convolution strategy.
 */
typedef DFTConvolution<SeFloat, PaddedImage<SeFloat, Reflect101Coordinates>> DefaultConvolutionStrategy;

} // end SExtractor


#endif // _SEFRAMEWORK_CONVOLUTION_CONVOLUTION_H
