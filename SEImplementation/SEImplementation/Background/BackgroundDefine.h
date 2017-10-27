/*
 * Created on Jan 05, 2015
 * @author: mkuemmel@usm.lmu.de
 *
 * Date:     $Date$
 * Revision: $Revision$
 * Author:   $Author$
 */
#ifndef BACKGROUNDDEFINE_H
#define	BACKGROUNDDEFINE_H

typedef float PIXTYPE;

#define BIG     1e+30   // a huge number
#define MAXCHAR 1048    //

/*------------------------------ field flags -------------------------------*/
#define		DETECT_FIELD	  0x0001  // Detection
#define		MEASURE_FIELD	  0x0002  // Measurement
#define		FLAG_FIELD	    0x0004  // Flagging
#define		RMS_FIELD	      0x0008  // Weighting with std deviations
#define		VAR_FIELD	      0x0010  // Weighting with variances
#define		WEIGHT_FIELD	  0x0020  // Weighting with weights
#define		BACKRMS_FIELD	  0x0040  // Weighting from a backrms matrix
#define		INTERP_FIELD	  0x0080  // Purely interpolated data
#define		MULTIGRID_FIELD	0x0100  // Multi-grid mode
/*----------------------  gap filling flags  --------------------------------*/
#define  MEAN  0x0200  // Mean of the minimum distance pixels  
#define  WEIGHTED_MEAN  0x0400  // Distance-weighted mean of the edge pixels  
/*----------------------------- Internal constants --------------------------*/
//#define BACK_BUFSIZE    1048576   /* bkgnd buffer */
#define BACK_BUFSIZE      1048576   // maximum size of the background buffer
#define BACK_MINGOODFRAC  0.5       // minimumm fraction of good values in a cell
#define BACK_FKAPPA       2.0       // kappa=nsigma for the first data cut
#define QUANTIF_NSIGMA    5         // kappa=nsigma for the histogram limits
#define QUANTIF_NMAXLEVELS  4096    // max nb of quantif. levels = size of the histogram
#define QUANTIF_AMIN    4           // min nb of "mode pixels"
#define BACK_EPS    (1e-4)          // a small number to end iteration

//#define BACK_WSCALE   1   /* Activate weight scaling */
//#define BACK_NOWSCALE   0   /* No weight scaling */

#ifndef PI
#define PI      3.1415926535898 // just for sure, maybe it is in C++??
#endif

#endif	/* BACKGROUNDDEFINE_H */
