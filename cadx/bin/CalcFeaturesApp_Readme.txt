
This application reads candidates from a CAN file, adds addition features, and produces a new CAN file that includes all of the features in the input CAN file plus the new ones.  If a HIT file is available for the image the user has the option of processing candidates that are true nodules first.  A HIT file can also be used as a source of candidates. In this case an input CAN file is required.



Usage:

-parm  parameterFile		# Parameter file
-outDir					# Directory to which log file and debug images are written.
-img  imageFile			# kespr image file
-mask maskFile				# Lung Mask file 
-inCan inCanFile			# Input CAN file
-outCan inCanFile			# Output CAN file
-log						# Write log file to outDir
-debug					# Write debug images to outDir
-hit hitFile				# HIT file for image  (optional)
-onlyHits					# Only process candidates in CAN file that are labeled definite or probable in HIT file.
-onlyCandidate n			# Only candidate n is processed. (optional)


Example:

CalcFeaturesApp -log -debug -parm V:/schildkr_CADX/ISTL/cadx/data/cadx4.parm -img L:/singhal/LungSegImages/All_CR/Images/CorrectOrientation/TGH-EK0208_CO.img -hit L:/singhal/LungSegImages/All_CR/GroundTruth/TGH-EK0208_CO.hit -mask L:/singhal/LungSegImages/All_CR/GroundTruth/lungGT/TGH-EK0208_CO.tif -inCan O:/cadx/testAll/IntermediateResults/initial_candidates_hand_masks/TGH-EK0208_CO.can -outDir Q:/schildkr/cadx/setB/run -outCan Q:/schildkr/cadx/setB/run/cadx.can



Installation:

The provided parameter file must be edited so that paths to all referenced files and directories are correct.  Note that if relative path names are used the names must be relative to the current working directory.




Jay Schildkraut
83697
