#include "angle.h"
#include "wsclean/wsclean.h"
#include "wsclean/logger.h"
#include "wscversion.h"
#include "numberlist.h"

#include <boost/algorithm/string.hpp>

#include <iostream>

void print_header();
void print_help();

#ifdef WSCLEAN_NO_MAIN
void print_header()
{
	Logger::Info <<
		"\n"
		"WSClean version " WSCLEAN_VERSION_STR " (" WSCLEAN_VERSION_DATE ")\n"
		"This software package is released under the GPL version 3.\n"
	  "Author: André Offringa (offringa@gmail.com).\n\n";
#ifndef NDEBUG
	Logger::Info << "\n"
		"WARNING: Symbol NDEBUG was not defined; this WSClean version was\n"
		"compiled as a DEBUG version. This can seriously affect performance!\n\n";
#endif
}

void print_help()
{
	std::cout << "Syntax: wsclean [options] <input-ms> [<2nd-ms> [..]]\n"
		"Will create cleaned images of the input ms(es).\n"
		"If multiple mses are specified, they need to be phase-rotated to the same point on the sky.\n\n"
		"Options can be:\n\n"
		"  ** GENERAL OPTIONS **\n"
		"-version\n"
		"   Print WSClean's version and exit.\n"
		"-j <threads>\n"
		"   Specify number of computing threads to use, i.e., number of cpu cores that will be used.\n"
		"   Default: use all cpu cores.\n"
		"-mem <percentage>\n"
		"   Limit memory usage to the given fraction of the total system memory. This is an approximate value.\n"
		"   Default: 100.\n"
		"-absmem <memory limit>\n"
		"   Like -mem, but this specifies a fixed amount of memory in gigabytes.\n"
		"-reorder\n"
		"-no-reorder\n"
		"   Force or disable reordering of Measurement Set. This can be faster when the measurement set needs to\n"
		"   be iterated several times, such as with many major iterations or in channel imaging mode.\n"
		"   Default: only reorder when in channel imaging mode.\n"
		"-tempdir <directory>\n"
		"   Set the temporary directory used when reordering files. Default: same directory as input measurement set.\n"
		"-saveweights\n"
		"   Save the gridded weights in the a fits file named <image-prefix>-weights.fits.\n"
		"-saveuv\n"
		"   Save the gridded uv plane, i.e., the FFT of the residual image. The UV plane is complex, hence\n"
		"   two images will be output: <prefix>-uv-real.fits and <prefix>-uv-imag.fits.\n"
		"-apply-primary-beam\n"
		"   Calculate and apply the primary beam and save images for the Jones components, with weighting identical to the\n"
		"   weighting as used by the imager. Only available for LOFAR.\n"
		"-reuse-primary-beam\n"
		"   If a primary beam image exists on disk, reuse those images (not implemented yet).\n"
		"-use-differential-lofar-beam\n"
		"   Assume the visibilities have already been beam-corrected for the reference direction.\n"
		"-update-model-required (default), and\n"
		"-no-update-model-required\n"
		"   These two options specify wether the model data column is required to\n"
		"   contain valid model data after imaging. It can save time to not update\n"
		"   the model data column.\n"
		"-verbose (or -v)\n"
		"   Increase verbosity of output.\n"
		"-log-time\n"
		"   Add date and time to each line in the output.\n"
		"-quiet\n"
		"   Do not output anything but errors.\n"
		"\n"
		"  ** WEIGHTING OPTIONS **\n"
		"-weight <weightmode>\n"
		"   Weightmode can be: natural, uniform, briggs. Default: uniform. When using Briggs' weighting,\n"
		"   add the robustness parameter, like: \"-weight briggs 0.5\".\n"
		"-superweight <factor>\n"
		"   Increase the weight gridding box size, similar to Casa's superuniform weighting scheme. Default: 1.0\n"
		"   The factor can be rational and can be less than one for subpixel weighting.\n"
		"-mfsweighting\n"
		"   In spectral mode, calculate the weights as if the image was made using MFS. This makes sure that the sum of\n"
		"   channel images equals the MFS weights. Otherwise, the channel image will become a bit more naturally weighted.\n"
		"   This is only relevant for weighting modes that require gridding (i.e., Uniform, Briggs').\n"
		"   Default: off, unless -joinchannels is specified.\n"
		"-nomfsweighting\n"
		"   Opposite of -mfsweighting; can be used to turn off MFS weighting in -joinchannels mode.\n"
		"-weighting-rank-filter <level>\n"
		"   Filter the weights and set high weights to the local mean. The level parameter specifies\n"
		"   the filter level; any value larger than level*localmean will be set to level*localmean.\n"
		"-weighting-rank-filter-size <size>\n"
		"   Set size of weighting rank filter. Default: 16.\n"
		"-taper-gaussian <beamsize>\n"
		"   Taper the weights with a Gaussian function. This will reduce the contribution of long baselines.\n"
		"   The beamsize is by default in asec, but a unit can be specified (\"2amin\").\n"
		"-taper-tukey <lambda>\n"
		"   Taper the outer weights with a Tukey transition. Lambda specifies the size of the transition; use in\n"
		"   combination with -maxuv-l.\n"
		"-taper-inner-tukey <lambda>\n"
		"   Taper the weights with a Tukey transition. Lambda specifies the size of the transition; use in\n"
		"   combination with -minuv-l.\n"
		"-taper-edge <lambda>\n"
		"   Taper the weights with a rectangle, to keep a space of lambda between the edge and gridded visibilities.\n"
		"-taper-edge-tukey <lambda>\n"
		"   Taper the edge weights with a Tukey window. Lambda is the size of the Tukey transition. When -taper-edge\n"
		"   is also specified, the Tukey transition starts inside the inner rectangle.\n"
		"\n"
		"  ** INVERSION OPTIONS **\n"
		"-name <image-prefix>\n"
		"   Use image-prefix as prefix for output files. Default is 'wsclean'.\n"
		"-size <width> <height>\n"
		"   Default: 2048 x 2048\n"
		"-trim <width> <height>\n"
		"   After inversion, trim the image to the given size.\n"
		"-scale <pixel-scale>\n"
		"   Scale of a pixel. Default unit is degrees, but can be specificied, e.g. -scale 20asec. Default: 0.01deg.\n"
		"-nwlayers <nwlayers>\n"
		"   Number of w-layers to use. Default: minimum suggested #w-layers for first MS.\n"
		"-nwlayers-for-size <width> <height>\n"
		"   Use the minimum suggested w-layers for an image of the given size. Can e.g. be used to increase\n"
		"   accuracy when predicting small part of full image. \n"
		"-channelsout <count>\n"
		"   Splits the bandwidth and makes count nr. of images. Default: 1.\n"
		"-predict\n"
		"   Only perform a single prediction for an existing image. Doesn't do any imaging or cleaning.\n"
		"   The input images should have the same name as the model output images would have in normal imaging mode.\n"
		"-predict-channels <nchannels>\n"
		"   Interpolate from a given number of images to the number of channels that are predicted\n"
		"   as specified by -channelsout. Will interpolate using the frequencies of the images.\n"
		"   Use one the -fit-spectral-... options to specify the interpolation method / freedom.\n"
		"   Only used when -predict is specified.\n"
		"-subtract-model\n"
		"   Subtract the model from the data column in the first iteration. This can be used to reimage\n"
		"   an already cleaned image, e.g. at a different resolution.\n"
		"-nosmallinversion and -smallinversion\n"
		"   Perform inversion at the Nyquist resolution and upscale the image to the requested image size afterwards.\n"
		"   This speeds up inversion considerably, but makes aliasing slightly worse. This effect is\n"
		"   in most cases <1%. Default: on.\n"
		"-gridmode <\"nn\", \"kb\" or \"rect\">\n"
		"   Kernel and mode used for gridding: kb = Kaiser-Bessel (default with 7 pixels), nn = nearest\n"
		"   neighbour (no kernel), rect = rectangular window. Default: kb.\n"
		"-gkernelsize <size>\n"
		"   Gridding antialiasing kernel size. Default: 7.\n"
		"-oversampling <factor>\n"
		"   Oversampling factor used during gridding. Default: 63.\n"
		"-make-psf\n"
		"   Always make the psf, even when no cleaning is performed.\n"
		"-make-psf-only\n"
		"   Only make the psf, no images are made.\n"
		"-savegridding\n"
		"   Save the gridding correction image. This shows the effect of the antialiasing filter. Default: not saved.\n"
		"-dft-prediction\n"
		"   Predict via a direct Fourier transform. This is slow, but can account for direction-dependent effects. This has\n"
		"   only effect when -mgain is set or -predict is given.\n"
		"-dft-with-beam\n"
		"   Apply the beam during DFT. Currently only works for LOFAR.\n"
		"-visibility-weighting-mode [normal/squared/unit]\n"
		"   Specify visibility weighting modi. Affects how the weights (normally) stored in\n"
		"   WEIGHT_SPECTRUM column are applied. Useful for estimating e.g. EoR power spectra errors.\n"
		"   Normally one would use this in combination with -no-normalize-for-weighting.\n"
		"-no-normalize-for-weighting\n"
		"   Disable the normalization for the weights, which makes the PSF's peak one. See\n"
		"   -visibility-weighting-mode. Only useful with natural weighting.\n"
		"\n"
		"  ** DATA SELECTION OPTIONS **\n"
		"-pol <list>\n"
		"   Default: \'I\'. Possible values: XX, XY, YX, YY, I, Q, U, V, RR, RL, LR or LL (case insensitive).\n"
		"   Multiple values can be separated with commas, e.g.: 'xx,xy,yx,yy'. Two or four polarizations can be\n"
		"   joinedly cleaned (see '-joinpolarizations'), but this is not the default. I, Q, U and V\n"
		"   polarizations will be directly calculated from the visibilities, which is not appropriate for\n"
		"   telescopes with non-orthogonal feeds, such as MWA and LOFAR. The 'xy' polarization will output both\n"
		"   a real and an imaginary image, which allows calculating true Stokes polarizations for those\n"
		"   telescopes.\n"
		"-interval <start-index> <end-index>\n"
		"   Only image the given time interval. Indices specify the timesteps, end index is exclusive.\n"
		"   Default: image all time steps.\n"
		"-intervalsout <count>\n"
		"   Number of intervals to image inside the selected global interval. Default: 1\n"
		"-channelrange <start-channel> <end-channel>\n"
		"   Only image the given channel range. Indices specify channel indices, end index is exclusive.\n"
		"   Default: image all channels.\n"
		"-field <fieldid>\n"
		"   Image the given field id. Default: first field (id 0).\n"
		"-datacolumn <columnname>\n"
		"   Default: CORRECTED_DATA if it exists, otherwise DATA will be used.\n"
		"-maxuvw-m <meters>\n"
		"-minuvw-m <meters>\n"
		"   Set the min/max baseline distance in meters.\n"
		"-maxuv-l <lambda>\n"
		"-minuv-l <lambda>\n"
		"   Set the min/max uv distance in lambda.\n"
		"-maxw <percentage>\n"
		"   Do not grid visibilities with a w-value higher than the given percentage of the max w, to save speed.\n"
		"   Default: grid everything\n"
		"\n"
		"  ** DECONVOLUTION OPTIONS **\n"
		"-niter <niter>\n"
		"   Maximum number of clean iterations to perform. Default: 0\n"
		"-threshold <threshold>\n"
		"   Stopping clean thresholding in Jy. Default: 0.0\n"
		"-gain <gain>\n"
		"   Cleaning gain: Ratio of peak that will be subtracted in each iteration. Default: 0.1\n"
		"-mgain <gain>\n"
		"   Cleaning gain for major iterations: Ratio of peak that will be subtracted in each major\n"
		"   iteration. To use major iterations, 0.85 is a good value. Default: 1.0\n"
		"-joinpolarizations\n"
		"   Perform cleaning by searching for peaks in the sum of squares of the polarizations (either\n"
		"   I^2+Q^2+U^2+V^2, XX^2+real(XY)^2+imag(XY)^2+YY^2, or XX^2+YY^2), but subtract components from\n"
		"   individual channels. Only possible when imaging all Stokes or all linear parameters. Default: off.\n"
		"-joinchannels\n"
		"   Perform cleaning by searching for peaks in the MFS image, but subtract components from individual channels.\n"
		"   This will turn on mfsweighting by default. Default: off.\n"
		"-multiscale\n"
		"   Clean on different scales. This is a new algorithm. Default: off.\n"
		"   This parameter invokes the v1.9 multiscale algorithm, which is slower but more accurate\n"
		"   compared to the older algorithm, and therefore the recommended one to use.\n"
		"   The older algorithm is now invoked with -fast-multiscale.\n"
		"-fast-multiscale\n"
		"   Clean on different scales. This is a new fast experimental algorithm. Default: off.\n"
		"   This method used to be invoked with -multiscale before v1.9, but the newer multiscale\n"
		"   algorithm is somewhat more accurate and therefore recommended.\n"
		"-multiscale-threshold-bias\n"
		"   Parameter to lower the threshold for larger scales. The used threshold for a scale\n"
		"   is threshold(scale)=pointsource_threshold x tbias^scale. A lower bias will clean\n"
		"   larger scales deeper. Default: 0.7\n"
		"-multiscale-scale-bias\n"
		"   Parameter to prevent cleaning small scales in the large-scale iterations. A higher\n"
		"   bias will give more focus to larger scales. Default: 0.6\n"
		"-multiscale-scales <comma-separated list of sizes in pixels>\n"
		"   Sets a list of scales to use in multi-scale cleaning. If unset, WSClean will select the delta\n"
		"   (zero) scale, scales starting at four times the synthesized PSF, and increase by a factor of\n"
		"   two until the maximum scale is reached. Example: -multiscale-scales 0,5,12.5\n"
		"-iuwt\n"
		"   Use the IUWT deconvolution algorithm.\n"
		"-moresane-ext <location>\n"
		"   Use the MoreSane deconvolution algorithm, installed at the specified location.\n"
		"-moresane-arg <arguments>\n"
		"   Pass the specified arguments to moresane. Note that multiple parameters have to be\n"
		"   enclosed in quotes.\n"
		"-moresane-sl <sl1,sl2,...>\n"
		"   MoreSane --sigmalevel setting for each major loop iteration. Useful to start at high\n"
		"   levels and go down with subsequent loops, e.g. 20,10,5\n"
		"-cleanborder <percentage>\n"
		"   Set the border size in which no cleaning is performed, in percentage of the width/height of the image.\n"
		"   With an image size of 1000 and clean border of 1%, each border is 10 pixels. Default: 5 (%).\n"
		"-fitsmask <mask>\n"
		"   Use the specified fits-file as mask during cleaning.\n"
		"-casamask <mask>\n"
		"   Use the specified CASA mask as mask during cleaning.\n"
		"-smallpsf\n"
		"   Resize the psf to speed up minor clean iterations. Not the default.\n"
		"-nonegative\n"
		"   Do not allow negative components during cleaning. Not the default.\n"
		"-negative\n"
		"   Default on: opposite of -nonegative.\n"
		"-stopnegative\n"
		"   Stop on negative components. Not the default.\n"
		"-fit-spectral-pol <nterms>\n"
		"   Fit a polynomial over frequency to each clean component. This has only effect\n"
		"   when the channels are joined with -joinchannels.\n"
		"-fit-spectral-log-pol <nterms>\n"
		"   Like fit-spectral-pol, but fits a logarithmic polynomial over frequency instead.\n"
		"-deconvolution-channels <nchannels>\n"
		"   Decrease the number of channels as specified by -channelsout to the given number for\n"
		"   deconvolution. Only possible in combination with one of the -fit-spectral options.\n"
		"   Proper residuals/restored images will only be returned when mgain < 1.\n"
		"\n"
		"  ** RESTORATION OPTIONS **\n"
		"-beamsize <arcsec>\n"
		"   Set the FWHM beam size in arcsec for restoring the clean components. Default: longest projected\n"
		"   baseline defines restoring beam.\n"
		"-beamshape <maj in arcsec> <min in arcsec> <position angle in deg>\n"
		"   Set the FWHM beam shape for restoring the clean components. Defaults units for maj and min are arcsec, and\n"
		"   degrees for PA. Can be overriden, e.g. '-beamshape 1amin 1amin 3deg'.\n"
		"-fitbeam\n"
		"   Determine beam shape by fitting the PSF (default if PSF is made).\n"
		"-nofitbeam\n"
		"   Do not determine beam shape from the PSF.\n"
		"-theoreticbeam\n"
		"   Write the beam in output fits files as calculated from the longest projected baseline.\n"
		"   This method results in slightly less accurate integrated fluxes, but in simple imaging provide\n"
		"   a beam size even without making the PSF. Default: off.\n"
		"-circularbeam\n"
		"   Force the beam to be circular: bmin will be set to bmaj.\n"
		"-ellipticalbeam\n"
		"   Allow the beam to be elliptical. Default.\n";
}
#endif

#ifdef WSCLEAN_NO_MAIN
int wsclean_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	if(argc < 2)
	{
		print_header();
		print_help();
		return -1;
	}
	
	WSClean wsclean;
	WSCleanSettings& settings = wsclean.Settings();;
	int argi = 1;
	bool mfsWeighting = false, noMFSWeighting = false, predictionMode = false;
	while(argi < argc && argv[argi][0] == '-')
	{
		const std::string param = argv[argi][1]=='-' ? (&argv[argi][2]) : (&argv[argi][1]);
		if(param == "version")
		{
			print_header();
			return 0;
		}
		else if(param == "quiet")
		{
			Logger::SetVerbosity(Logger::QuietVerbosity);
		}
		else if(param == "v" || param == "verbose")
		{
			Logger::SetVerbosity(Logger::VerboseVerbosity);
		}
		else if(param == "log-time")
		{
			Logger::SetLogTime(true);
		}
		else if(param == "tempdir")
		{
			++argi;
			settings.temporaryDirectory = argv[argi];
		}
		else if(param == "saveweights")
		{
			settings.isWeightImageSaved = true;
		}
		else if(param == "saveuv")
		{
			settings.isUVImageSaved = true;
		}
		else if(param == "predict")
		{
			predictionMode = true;
		}
		else if(param == "predict-channels")
		{
			++argi;
			settings.predictionChannels = atoi(argv[argi]);
		}
		else if(param == "subtract-model")
		{
			settings.subtractModel = true;
		}
		else if(param == "size")
		{
			size_t
				width = atoi(argv[argi+1]),
				height = atoi(argv[argi+2]);
			settings.untrimmedImageWidth = width;
			settings.untrimmedImageHeight = height;
			argi += 2;
		}
		else if(param == "trim")
		{
			settings.trimmedImageWidth = atoi(argv[argi+1]);
			settings.trimmedImageHeight = atoi(argv[argi+2]);
			argi += 2;
		}
		else if(param == "scale")
		{
			++argi;
			settings.pixelScaleX = Angle::Parse(argv[argi], "scale parameter", Angle::Degrees);
			settings.pixelScaleY = settings.pixelScaleX;
		}
		else if(param == "nwlayers")
		{
			++argi;
			settings.nWLayers = atoi(argv[argi]);
		}
		else if(param == "nwlayers-for-size")
		{
			settings.widthForNWCalculation = atoi(argv[argi+1]);
			settings.heightForNWCalculation = atoi(argv[argi+2]);
			argi += 2;
		}
		else if(param == "gain")
		{
			++argi;
			settings.deconvolutionGain = atof(argv[argi]);
		}
		else if(param == "mgain")
		{
			++argi;
			settings.deconvolutionMGain = atof(argv[argi]);
		}
		else if(param == "niter")
		{
			++argi;
			settings.deconvolutionIterationCount = atoi(argv[argi]);
		}
		else if(param == "threshold")
		{
			++argi;
			settings.deconvolutionThreshold = atof(argv[argi]);
		}
		else if(param == "datacolumn")
		{
			++argi;
			settings.dataColumnName = argv[argi];
		}
		else if(param == "pol")
		{
			++argi;
			settings.polarizations = Polarization::ParseList(argv[argi]);
		}
		else if(param == "apply-primary-beam")
		{
			settings.applyPrimaryBeam = true;
		}
		else if(param == "reuse-primary-beam")
		{
			settings.reusePrimaryBeam = true;
		}
		else if(param == "use-differential-lofar-beam")
		{
			settings.useDifferentialLofarBeam = true;
		}
		else if(param == "negative")
		{
			settings.allowNegativeComponents = true;
		}
		else if(param == "nonegative")
		{
			settings.allowNegativeComponents = false;
		}
		else if(param == "stopnegative")
		{
			settings.stopOnNegativeComponents = true;
		}
		else if(param == "iuwt")
		{
			settings.useIUWTDeconvolution = true;
			// Currently (WSClean 1.9, 2015-08-19) IUWT deconvolution
			// seems not to work when allowing negative components. The algorithm
			// becomes unstable. Hence, turn negative components off.
			settings.allowNegativeComponents = false;
		}
		else if(param == "moresane-ext")
		{
			++argi;
			settings.useMoreSaneDeconvolution = true;
			settings.moreSaneLocation = argv[argi];
		}
		else if(param == "moresane-arg")
		{
			++argi;
			settings.moreSaneArgs = argv[argi];
		}
		else if(param == "moresane-sl")
		{
			++argi;
			NumberList::ParseDoubleList(argv[argi], settings.moreSaneSigmaLevels);
		}
		else if(param == "make-psf")
		{
			settings.makePSF = true;
		}
		else if(param == "make-psf-only")
		{
			settings.makePSFOnly = true;
		}
		else if(param == "savegridding")
		{
			settings.isGriddingImageSaved = true;
		}
		else if(param == "dft-prediction")
		{
			settings.dftPrediction = true;
		}
		else if(param == "dft-with-beam")
		{
			settings.dftWithBeam = true;
		}
		else if(param == "name")
		{
			++argi;
			settings.prefixName = argv[argi];
		}
		else if(param == "gridmode")
		{
			++argi;
			std::string gridModeStr = argv[argi];
			boost::to_lower(gridModeStr);
			if(gridModeStr == "kb" || gridModeStr == "kaiserbessel" || gridModeStr == "kaiser-bessel")
				settings.gridMode = KaiserBesselKernel;
			else if(gridModeStr == "rect")
				settings.gridMode = RectangularKernel;
			else if(gridModeStr == "nn" || gridModeStr == "nearestneighbour")
				settings.gridMode = NearestNeighbourGridding;
			else
				throw std::runtime_error("Invalid gridding mode: should be either kb (Kaiser-Bessel) or nn (NearestNeighbour)");
		}
		else if(param == "smallinversion")
		{
			settings.smallInversion = true;
		}
		else if(param == "nosmallinversion")
		{
			settings.smallInversion = false;
		}
		else if(param == "interval")
		{
			settings.startTimestep = atoi(argv[argi+1]);
			settings.endTimestep = atoi(argv[argi+2]);
			argi += 2;
		}
		else if(param == "intervalsout")
		{
			++argi;
			settings.intervalsOut = atoi(argv[argi]);
		}
		else if(param == "channelrange")
		{
			settings.startChannel = atoi(argv[argi+1]);
			settings.endChannel = atoi(argv[argi+2]);
			argi += 2;
		}
		else if(param == "channelsout")
		{
			++argi;
			settings.channelsOut = atoi(argv[argi]);
		}
		else if(param == "joinpolarizations")
		{
			settings.joinedPolarizationCleaning = true;
		}
		else if(param == "joinchannels")
		{
			settings.joinedFrequencyCleaning = true;
		}
		else if(param == "mfsweighting")
		{
			mfsWeighting = true;
		}
		else if(param == "taper-gaussian")
		{
			++argi;
			double taperBeamSize = Angle::Parse(argv[argi], "Gaussian taper", Angle::Arcseconds);
			settings.gaussianTaperBeamSize = taperBeamSize;
		}
		else if(param == "taper-edge")
		{
			++argi;
			settings.edgeTaperInLambda = atof(argv[argi]);
		}
		else if(param == "taper-edge-tukey")
		{
			++argi;
			settings.edgeTukeyTaperInLambda = atof(argv[argi]);
		}
		else if(param == "taper-tukey")
		{
			++argi;
			settings.tukeyTaperInLambda = atof(argv[argi]);
		}
		else if(param == "taper-inner-tukey")
		{
			++argi;
			settings.tukeyInnerTaperInLambda = atof(argv[argi]);
		}
		else if(param == "multiscale")
		{
			settings.useMultiscale = true;
		}
		else if(param == "fast-multiscale")
		{
			settings.useFastMultiscale = true;
		}
		else if(param == "multiscale-threshold-bias")
		{
			++argi;
			settings.multiscaleDeconvolutionThresholdBias = atof(argv[argi]);
		}
		else if(param == "multiscale-scale-bias")
		{
			++argi;
			settings.multiscaleDeconvolutionScaleBias = atof(argv[argi]);
		}
		else if(param == "multiscale-normalize-response")
		{
			++argi;
			settings.multiscaleNormalizeResponse = true;
		}
		else if(param == "multiscale-scales")
		{
			++argi;
			NumberList::ParseDoubleList(argv[argi], settings.multiscaleScaleList);
		}
		else if(param == "weighting-rank-filter")
		{
			++argi;
			settings.rankFilterLevel = atof(argv[argi]);
		}
		else if(param == "weighting-rank-filter-size")
		{
			++argi;
			settings.rankFilterSize = atoi(argv[argi]);
		}
		else if(param == "cleanborder")
		{
			++argi;
			settings.deconvolutionBorderRatio = atof(argv[argi])*0.01;
		}
		else if(param == "fitsmask")
		{
			++argi;
			settings.fitsDeconvolutionMask = argv[argi];
		}
		else if(param == "casamask")
		{
			++argi;
			settings.casaDeconvolutionMask = argv[argi];
		}
		else if(param == "nomfsweighting")
		{
			noMFSWeighting = true;
		}
		else if(param == "joinchannels")
		{
			settings.joinedFrequencyCleaning = true;
		}
		else if(param == "fit-spectral-pol")
		{
			++argi;
			settings.spectralFittingMode = PolynomialSpectralFitting;
			settings.spectralFittingTerms = atoi(argv[argi]);
		}
		else if(param == "fit-spectral-log-pol")
		{
			++argi;
			settings.spectralFittingMode = LogPolynomialSpectralFitting;
			settings.spectralFittingTerms = atoi(argv[argi]);
		}
		else if(param == "deconvolution-channels")
		{
			++argi;
			settings.deconvolutionChannelCount = atoi(argv[argi]);
		}
		else if(param == "field")
		{
			++argi;
			settings.fieldId = atoi(argv[argi]);
		}
		else if(param == "weight")
		{
			++argi;
			std::string weightArg = argv[argi];
			if(weightArg == "natural")
				settings.weightMode = WeightMode::NaturalWeighted;
			else if(weightArg == "uniform")
				settings.weightMode = WeightMode::UniformWeighted;
			else if(weightArg == "briggs")
			{
				++argi;
				settings.weightMode = WeightMode::Briggs(atof(argv[argi]));
			}
			else throw std::runtime_error("Unknown weighting mode specified");
		}
		else if(param == "superweight")
		{
			++argi;
			settings.weightMode.SetSuperWeight(atof(argv[argi]));
		}
		else if(param == "beamsize")
		{
			++argi;
			double beam = Angle::Parse(argv[argi], "beam size", Angle::Arcseconds);
			settings.manualBeamMajorSize = beam;
			settings.manualBeamMinorSize = beam;
			settings.manualBeamPA = 0.0;
		}
		else if(param == "beamshape")
		{
			double beamMaj = Angle::Parse(argv[argi+1], "beam shape, major axis", Angle::Arcseconds);
			double beamMin = Angle::Parse(argv[argi+2], "beam shape, minor axis", Angle::Arcseconds);
			double beamPA = Angle::Parse(argv[argi+3], "beam shape, position angle", Angle::Degrees);
			argi+=3;
			settings.manualBeamMajorSize = beamMaj;
			settings.manualBeamMinorSize = beamMin;
			settings.manualBeamPA = beamPA;
		}
		else if(param == "fitbeam")
		{
			settings.fittedBeam = true;
		}
		else if(param == "nofitbeam")
		{
			settings.fittedBeam = false;
		}
		else if(param == "theoreticbeam")
		{
			settings.theoreticBeam = true;
			settings.fittedBeam = false;
		}
		else if(param == "circularbeam")
		{
			settings.circularBeam = true;
		}
		else if(param == "ellipticalbeam")
		{
			settings.circularBeam = false;
		}
		else if(param == "gkernelsize")
		{
			++argi;
			settings.antialiasingKernelSize = atoi(argv[argi]);
		}
		else if(param == "oversampling")
		{
			++argi;
			settings.overSamplingFactor = atoi(argv[argi]);
		}
		else if(param == "reorder")
		{
			settings.forceReorder = true;
			settings.forceNoReorder = false;
		}
		else if(param == "no-reorder")
		{
			settings.forceNoReorder = true;
			settings.forceReorder = false;
		}
		else if(param == "update-model-required")
		{
			settings.modelUpdateRequired = true;
		}
		else if(param == "no-update-model-required")
		{
			settings.modelUpdateRequired = false;
		}
		else if(param == "j")
		{
			++argi;
			settings.threadCount = atoi(argv[argi]);
		}
		else if(param == "mem")
		{
			++argi;
			settings.memFraction = atof(argv[argi]) / 100.0;
		}
		else if(param == "absmem")
		{
			++argi;
			settings.absMemLimit = atof(argv[argi]);
		}
		else if(param == "maxuvw-m")
		{
			++argi;
			settings.maxUVWInMeters = atof(argv[argi]);
		}
		else if(param == "minuvw-m")
		{
			++argi;
			settings.minUVWInMeters = atof(argv[argi]);
		}
		else if(param == "maxuv-l")
		{
			++argi;
			settings.maxUVInLambda = atof(argv[argi]);
		}
		else if(param == "minuv-l")
		{
			++argi;
			settings.minUVInLambda = atof(argv[argi]);
		}
		else if(param == "maxw")
		{
			// This was to test the optimization suggested in Tasse et al., 2013, Appendix C.
			++argi;
			settings.wLimit = atof(argv[argi]);
		}
		else if(param == "no-normalize-for-weighting")
		{
			settings.normalizeForWeighting = false;
		}
		else if(param == "visibility-weighting-mode")
		{
			++argi;
			std::string modeStr = argv[argi];
			boost::to_lower(modeStr);
			if(modeStr == "normal")
				settings.visibilityWeightingMode = MeasurementSetGridder::NormalVisibilityWeighting;
			else if(modeStr == "squared")
				settings.visibilityWeightingMode = MeasurementSetGridder::SquaredVisibilityWeighting;
			else if(modeStr == "unit")
				settings.visibilityWeightingMode = MeasurementSetGridder::UnitVisibilityWeighting;
			else
				throw std::runtime_error("Unknown weighting mode: " + modeStr);
		}
		else if(param == "use-idg")
		{
			settings.useIDG = true;
		}
		else {
			throw std::runtime_error("Unknown parameter: " + param);
		}
		
		++argi;
	}
	
	if(argi == argc)
		throw std::runtime_error("No input measurement sets given.");
	
	// Done parsing.
	
	// We print the header only now, because the logger has now been set up
	// and possibly set to quiet.
	print_header();
	
	settings.mfsWeighting = (settings.joinedFrequencyCleaning && !noMFSWeighting) || mfsWeighting;
	
	for(int i=argi; i != argc; ++i)
		settings.filenames.push_back(argv[i]);
	
	std::ostringstream commandLineStr;
	commandLineStr << "wsclean";
	for(int i=1; i!=argc; ++i)
		commandLineStr << ' ' << argv[i];
	wsclean.SetCommandLine(commandLineStr.str());
	
	settings.Validate();
	
	if(predictionMode)
		wsclean.RunPredict();
	else
		wsclean.RunClean();
	return 0;
}
