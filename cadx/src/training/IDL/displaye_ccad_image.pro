;	#^DET V1.0
;	operatingFPI: 2.9574468
;	operatingTPF: 0.32786885
;	operatingAimFPI: 3
;	trainingFileID: G:\detectionalysis\Debug\November172003\setA_groundtruth\november172003_setA.trn
;	numImages: 47
;	imageID: EK0002_0_PA_P_CR400_CO.img
;	imageNumCandidates: 4
;	4
;	6
;	7
;	10

function get_detection_indices, detfilename, imagefilename, numcandidates, candidates

	ts = strarr(1)
	num_expected_points = 0L

	;============================================================================
	; Open the file for reading.
	;============================================================================
	openr, luninp, detfilename, /get_lun
	if(luninp lt 0) then return, 0

	;============================================================================
	; Check to see that this is actually a can file.
	;============================================================================
	readf, luninp, ts
	if(strcmp(ts[0], '#^DET V1.0', strlen('#^DET V1.0')) eq 0) then begin
		return, 0
	endif

	targetstring = 'imageID: ' + imagefilename

	numcandidates = 0
	;============================================================================
	; Read in the rest of the data from the file.
	;============================================================================
	while(not eof(luninp)) do begin
		readf, luninp, ts
		if(strcmp(ts[0],targetstring,strlen(targetstring)) eq 1) then begin
			readf, luninp, ts
			if(strcmp(ts[0],'imageNumCandidates: ',strlen('imageNumCandidates: ')) eq 1) then begin
				sizevals = long(strsplit(strmid(ts[0], strlen('imageNumCandidates: '), $
					strlen(ts[0]) - strlen('imageNumCandidates: ')),' ', /extract))
				numcandidates = sizevals[0]
				for j=0L,numcandidates-1 do begin
					if(j eq 0) then candidates = intarr(numcandidates)
					readf, luninp, ts
					;print, fix(ts[0])
					candidates[j] = fix(ts[0])
				endfor
				free_lun, luninp
				return, 1
			endif
		endif
	endwhile

	free_lun, luninp
	return, 1
end

function display_ccad_image, tiffimagefilename, hitfilename=hitfilename, canfilename=canfilename, $
	detfilename=detfilename, displaytruthflag=displaytruthflag, inputdownsamplefactor=inputdownsamplefactor, $
	whichimage=whichimage, displaymodemode=displaymode

	device, decompose=0

	textsize = 1

	;==========================================================================
	; Read the TIFF image from the file.  This is assumed to be a 12-bit
	; image in P-values.
	;==========================================================================
	if(strmatch(tiffimagefilename, '*.tif') ne 0) then begin
		orientation=0
		image_16bit = read_tiff(tiffimagefilename, orientation=orientation)
		divval = 16
	endif else begin
		image_16bit = read_dicom(string(byte(tiffimagefilename)))
		divval = 16
	endelse

	if(displaymode eq 0) then begin

		downsamplefactor = inputdownsamplefactor

		if(n_elements(downsamplefactor) ne 0) then begin
			if(downsamplefactor ne 1) then begin
				s = size(image_16bit, /dimensions)
			    origcols = s[0]
   			 	origrows = s[1]
   			 	cols = origcols / downsamplefactor
		   	 	rows = origrows / downsamplefactor
		   	 	if((downsamplefactor eq 1) or (downsamplefactor eq 2) or (downsamplefactor eq 4)) then begin
					image_16bit = rebin(temporary(image_16bit), cols, rows)
				endif else begin
					image_16bit = congrid(temporary(image_16bit), cols, rows)
				endelse
				scale = double(cols) / double(origcols)
				simscale = 1
				if(downsamplefactor eq 2) then begin
					textsize = 2
					simscale = 2
				endif
			endif else begin
				s = size(image_16bit, /dimensions)
				origcols = s[0]
				origrows = s[1]
				cols = s[0]
   			 	rows = s[1]
   			 	scale = 1.0
   			 	simscale = 3
	   		 	textsize = 3
			endelse
		endif else begin
			s = size(image_16bit, /dimensions)
			origcols = s[0]
			origrows = s[1]
			cols = s[0]
   		 	rows = s[1]
   		 	scale = 1.0
   		 	simscale = 3
   		 	textsize = 3
		endelse

		if((rows gt 1024) or (cols gt 1200)) then begin
			if((rows > cols) gt 1250) then begin
				if(whichimage eq 1) then return, -1
				xpos = 0
				ypos = 0
			endif else begin
				if(whichimage eq 1) then begin
					xpos = 0
					ypos = 0
				endif else begin
					if(cols le rows) then begin
						xpos = cols
						ypos = 0
					endif else begin
						xpos = 0
						ypos = rows
					endelse
				endelse
			endelse
			window, xsize=cols, ysize=rows, xpos=1280+xpos, ypos=ypos-1370, title=tiffimagefilename, /free
		endif else begin
			if(whichimage eq 1) then begin
				xpos = 0
				ypos = 0
			endif else begin
				if(cols le rows) then begin
					xpos = cols
					ypos = 0
				endif else begin
					xpos = 0
					ypos = rows
				endelse
			endelse
			window, xsize=cols, ysize=rows, xpos=xpos, ypos=ypos, title=tiffimagefilename, /free
		endelse
	endif else begin
		if(whichimage eq 1) then begin
			xpos = 1280
			ypos = -1370
			scale = 1.0
			simscale = 4
			downsamplefactor = 1
			s = size(image_16bit, /dimensions)
		    origcols = s[0]
   		 	origrows = s[1]
   		 	cols = origcols / downsamplefactor
	   	 	rows = origrows / downsamplefactor
	   	 	textsize = 4
		endif else begin
			downsamplefactor = 4
			s = size(image_16bit, /dimensions)
		    origcols = s[0]
   		 	origrows = s[1]
   		 	cols = origcols / downsamplefactor
	   	 	rows = origrows / downsamplefactor
	   	 	image_16bit = rebin(temporary(image_16bit), cols, rows)
			xpos = 1280 - cols
			ypos = 0
			scale = double(cols) / double(origcols)
			simscale = 1
		endelse
		window, xsize=cols, ysize=rows, xpos=xpos, ypos=ypos, title=tiffimagefilename, /free
	endelse

	thewindowid = !d.window

	tv, byte(image_16bit/divval), /order

	if(n_elements(hitfilename) ne 0) then begin

		;==========================================================================
		; Read in the health imaging truth file.
		;==========================================================================
		status = read_hit_file(hitfilename, hit_data=hit_data)

		;============================================================================
		; If the ground truth is present, draw it on the display.
		;============================================================================
		if(n_elements(displaytruthflag) ne 0) then begin
			if(status eq 1) then begin
				for i=0L,hit_data.numNodules-1 do begin
					;print, 'noduleIndex: ', strtrim(string((*hit_data.nodule)[i].index),2)
					;print, 'status: ', *(*hit_data.nodule)[i].status
					;print, 'centroid: ' + $
					;	strtrim(string((*hit_data.nodule)[i].centroidx),2) + ' ' + $
					;	strtrim(string((*hit_data.nodule)[i].centroidy),2)
					;print, 'nPoints: ' + strtrim(string((*hit_data.nodule)[i].numpoints),2)
					if(((*(*hit_data.nodule)[i].status) eq 'definite') or ((*(*hit_data.nodule)[i].status) eq 'probable') or ((*(*hit_data.nodule)[i].status) eq 'possible') $
						)then begin
						;for j=0L,(*hit_data.nodule)[i].numpoints-1 do begin
						;	printf, lunout, strtrim(string((*(*hit_data.nodule)[i].xycoords)[0,j]),2) + $
						;	' ' + strtrim(string((*(*hit_data.nodule)[i].xycoords)[1,j]),2)
						;endfor
						plots, scale * ((*(*hit_data.nodule)[i].xycoords)[0,*]), scale * (origrows-1-(*(*hit_data.nodule)[i].xycoords)[1,*]), color=255, /device, thick=3, linestyle=5
						text_xpos = (total((*(*hit_data.nodule)[i].xycoords)[0,*]) / n_elements((*(*hit_data.nodule)[i].xycoords)[0,*]))
						text_ypos = (min((*(*hit_data.nodule)[i].xycoords)[1,*]) - 20)
						c = 2
						xyouts, scale * text_xpos, scale * (origrows-text_ypos), strtrim(string((*hit_data.nodule)[i].index),2) + ':' + (*(*hit_data.nodule)[i].status), charsize=textsize, charthick=textsize, alignment=0.5, font=-1, /device
					endif
			   endfor
			endif
   		endif
	endif

	;==========================================================================
	; Read in the candidate data.
	;==========================================================================
	if(n_elements(canfilename) eq 0) then return, thewindowid

	status = read_can_file_v11(canfilename, can_data=can_data)

	status = get_detection_indices(detfilename, *can_data.xraySourceID, numcandidates, candidates)

	if(numcandidates gt 0) then begin
		print,'Detections:'
		print, candidates
	endif else begin
		print, 'No Detections'
	endelse

	;============================================================================
	; If the candidate data is present, draw it on the display.
	;============================================================================
	if(status eq 1) then begin
		text_xpos = (origcols) / 2
		text_ypos = (origrows - 200)
		xyouts, scale * text_xpos, scale * (origrows-text_ypos), strtrim(string(numcandidates),2) + ' ROIs Found', charsize=textsize, charthick=textsize, alignment=0.5, font=-1, /device
		for i=0L,can_data.numCandidates-1 do begin
			;print, 'candidateIndex: ', strtrim(string((*can_data.candidate)[i].index),2)
			;print, 'centroid: ' + $
			;	strtrim(string((*can_data.candidate)[i].centroidx),2) + ' ' + $
			;	strtrim(string((*can_data.candidate)[i].centroidy),2)
			;print, 'nPoints: ' + strtrim(string((*can_data.candidate)[i].numpoints),2)
			;for j=0L,(*hit_data.nodule)[i].numpoints-1 do begin
			;	printf, lunout, strtrim(string((*(*hit_data.nodule)[i].xycoords)[0,j]),2) + $
			;	' ' + strtrim(string((*(*hit_data.nodule)[i].xycoords)[1,j]),2)
			;endfor
			;text_xpos = total((*(*hit_data.nodule)[i].xycoords)[0,*]) / n_elements((*(*hit_data.nodule)[i].xycoords)[0,*])
			;text_ypos = min((*(*hit_data.nodule)[i].xycoords)[1,*]) - 20
			;xyouts, text_xpos, rows-text_ypos, (*(*hit_data.nodule)[i].status), charsize=2.0, charthick=3.0, alignment=0.5, /device
			thisindex = (*can_data.candidate)[i].index
			if(numcandidates eq 0) then begin
				count = 0
			endif else begin
				indx = where(candidates eq thisindex, count)
			endelse
			if(count ne 0) then begin
				;print, 'Here'
				;help, count
				t = (indgen(49, /double) / 48.0) * 2.0 * 3.1415926535
				usersym, simscale * cos(5 * t) * cos(t), simscale * cos(5 * t) * sin(t)
				plots, scale * ((*can_data.candidate)[i].centroidx), scale * (origrows-1-(*can_data.candidate)[i].centroidy), /device, psym=2, symsize=simscale, color=255
			;	print, 'Plotting ' + strtrim(string(thisindex),2)
	;DS			circle_x = (((*can_data.candidate)[i].centroidx) + (((3.5 / 0.0171) / 2.0) * cos((indgen(501, /double) / 500.0) * (!pi * 2.0))))
	;DS			circle_y = (((*can_data.candidate)[i].centroidy) + (((3.5 / 0.0171) / 2.0) * sin((indgen(501, /double) / 500.0) * (!pi * 2.0))))
	;DS			plots, scale * circle_x, scale * (origrows-1-(circle_y)), color=255, /device, linestyle=2
				if((*can_data.candidate)[i].numpoints gt 0) then begin
					plots, scale * ((*(*can_data.candidate)[i].xycoords)[0,*]), scale * (origrows-1-(*(*can_data.candidate)[i].xycoords)[1,*]), color=255, /device, psym=3   ;thick=1, linestyle=0
				endif
			endif else begin
				t = (indgen(49, /double) / 48.0) * 2.0 * 3.1415926535
				usersym, cos(t), sin(t)
				plots, scale * ((*can_data.candidate)[i].centroidx), scale * (origrows-1-(*can_data.candidate)[i].centroidy), /device, psym=8, symsize=0.25*simscale, color=255
			endelse
	   endfor
   endif

	free_can_data, can_data

	return, thewindowid

end

function trimpathandsuffix, filelist

	for i=0L,n_elements(filelist)-1 do begin
		startpos = strpos(filelist[i], '\', /reverse_search)
		endpos = strpos(filelist[i], '.')
		if(startpos lt 0) then startpos = 0 else startpos = startpos + 1
		if(endpos lt 0) then endpos = strlen(filelist[i]) - 1
		tempstring = strmid(filelist[i], startpos, endpos - startpos)
		if(i eq 0) then trimmedlist = tempstring else trimmedlist = [trimmedlist, tempstring]
	endfor

	return, trimmedlist

end

pro test_display_ccad_image, reduce=reduce, displaymode=displaymode, settoview=settoview

;	dir_image = 'G:\ChestCad\Test11\Images\IPL_handprocessed\setA\'
;	dir_hit = 'G:\ChestCad\Test13\GroundTruth\'
;	dir_can = 'G:\ChestCad\Test13\IntermediateResults\SegmentationFeatures\'
;	fndet = 'D:\test_classification.out'

	image_extention = '*.tif'

	if(0) then begin
		dir_image = 'G:\ChestCad\Test17\Images\'
		dir_hit = 'G:\ChestCad\Test17\GroundTruth\'
		dir_can = 'G:\ChestCad\Test17\IntermediateResults\SegmentationFeatures\'
		fndet = 'G:\ChestCad\Test17\testdet.txt'
	endif

	;--------------------------------------------------------------------------
	; Inputs for Viewing SetA processed with the December042003 algorithms.
	;--------------------------------------------------------------------------
	if(settoview eq 3) then begin
		print, 'DISPLAYING DECEMBER042003 RESULTS.........'
		dir_image = 'G:\ChestCad\IPL_Processed_Tiff\SetA\'
		;dir_hit = 'G:\detectionalysis\Debug\December112003_Review\November262003\hit_SetA\'
		dir_hit = 'G:\detectionalysis\Debug\OrigHitFiles_for_SetA\'
		dir_can = 'G:\detectionalysis\Debug\December112003_Review\December042003\December042003_SetA_can\'
		fndet = 'G:\detectionalysis\Debug\December112003_Review\December042003\December042003_SetA_bestGML_classificationresults.txt'
	endif

	;--------------------------------------------------------------------------
	; Inputs for Viewing SetA processed with the March262004 algorithms.   ; November262003 algorithms.
	;--------------------------------------------------------------------------
	if(settoview eq 2) then begin
		print, 'DISPLAYING NOVEMBER262003 RESULTS.........'
		dir_image = 'G:\ChestCad\IPL_Processed_Tiff\SetA\'
		;dir_hit = 'G:\detectionalysis\Debug\December112003_Review\November262003\hit_SetA\'
		;dir_can = 'G:\detectionalysis\Debug\December112003_Review\November262003\November262003_SetA_can\'
		dir_hit = 'L:\chestCad\Development\DataSets\SetB\GroundTruth\'
		dir_can = 'G:\detectionalysis\Debug\2004\March262004\CanFiles\SetB\'
		;fndet = 'G:\detectionalysis\Debug\December112003_Review\November262003\november262003_SetA_best18GML_classificationresults.txt'
		fndet = 'G:\March262004_SetB_GML9feature_basefn_classificationresults.txt'
		;fndet = 'G:\jay_features_classificationresults.txt'
	endif

	;--------------------------------------------------------------------------
	; Inputs for viewing 137 with automatic lung segmentation.
	;--------------------------------------------------------------------------
	if(settoview eq 0) then begin
		print, 'DISPLAYING JULY242003 RESULTS.........'
		dir_image = 'G:\ChestCad\Test11\Images\IPL_handprocessed\'
		dir_hit = 'G:\ChestCad\Test16\GroundTruth\'
		dir_can = 'G:\detectionalysis\Debug\December112003_Review\July242003\137_can\'
		fndet = 'G:\detectionalysis\Debug\December112003_Review\July242003\July242003_137_bestGML_classificationresults.txt'
	endif

	;--------------------------------------------------------------------------
	; Inputs for viewing 137 with manual (hand drawn) lung segmentation.
	;--------------------------------------------------------------------------
	if(settoview eq 1) then begin
		print, 'DISPLAYING JULY242003 (MANUAL LUNG SEGMENTATION) RESULTS.........'
		dir_image = 'G:\ChestCad\Test11\Images\IPL_handprocessed\'
		dir_hit = 'G:\ChestCad\Test16\GroundTruth\'
		dir_can = 'G:\ChestCad\Test16\IntermediateResults\SegmentationFeatures\stripped\'
		fndet = 'G:\detectionalysis\Debug\December112003_Review\July242003_MLM\July242003_137_MLM_best13_classificationresults.txt'
	endif

	;--------------------------------------------------------------------------
	; Inputs for viewing Januuary272004_MLM_SetB_w/culling
	;--------------------------------------------------------------------------
	if(settoview eq 1) then begin
		print, 'DISPLAYING JANUARY272003 (MANUAL LUNG SEGMENTATION) RESULTS (Fselect w cull rune wo/cull).........'
		dir_image = 'G:\ChestCad\IPL_Processed_Tiff\SetA\'
		dir_hit = 'L:\chestCad\Development\DataSets\SetB\GroundTruth\'
		dir_can = 'G:\detectionalysis\Debug\2004\January272004\CanFiles\'
		;fndet = 'G:\GML_January272004_MLM_SetB_Cull0woCULL_base_classificationresults.txt'
		fndet = 'G:\GML_January272004_MLM_SetB_Cull0wCULL_base_classificationresults.txt'
	endif


	;--------------------------------------------------------------------------
	; Display the truth on the external testing images. Those image have the
	; detections already burned into them.
	;--------------------------------------------------------------------------
	if((settoview ge 11) and (settoview le 16)) then begin
		dir_hit = 'L:\chestCad\Development\DataSets\SetB\GroundTruth\'
		if(settoview eq 11) then dir_image = 'H:\Deus\Marked\Condition1\OriginalOrientation\'
		if(settoview eq 12) then dir_image = 'H:\Deus\Marked\Condition2\OriginalOrientation\'
		if(settoview eq 13) then dir_image = 'H:\Deus\Marked\Condition3\OriginalOrientation\'
		if(settoview eq 14) then dir_image = 'H:\Deus\Marked\Condition4\OriginalOrientation\'
		if(settoview eq 15) then dir_image = 'H:\Deus\Marked\Condition5\OriginalOrientation\'
		if(settoview eq 16) then dir_image = 'H:\Deus\Marked\Condition6\OriginalOrientation\'
		image_extention = '*.dcm'
	endif

	;--------------------------------------------------------------------------
	; Inputs for viewing July112004 SetA featureselection and train/test
	; results with viewing the truth from SetB. This was done for an analysis
	; for the July 19, 2004 review to show the difference in performance
	; made over time. These results are directly comparable to the December 2003
	; review meeting results.
	;--------------------------------------------------------------------------
	if(settoview eq 200407191) then begin
		print, 'DISPLAYING JULY112004 SetA (AUTO LUNG SEGMENTATION) RESULTS with SETB truth.........'
		dir_image = 'G:\ChestCad\IPL_Processed_Tiff\SetA\'
		dir_hit = 'L:\chestCad\Development\DataSets\SetB\GroundTruth\'
		dir_can = 'G:\detectionalysis\Debug\2004\July112004\SetA\CanFiles\'
		fndet = 'G:\detectionalysis\Debug\2004\July112004\SetA\OutCan\GML_KNNrunoutput__classificationresults.txt'
	endif

	;--------------------------------------------------------------------------
	; Inputs for viewing July112004 SetB featureselection and train/test
	; results with viewing the truth from SetB. This was done for an analysis
	; for the July 19, 2004 review to show the difference in performance
	; made over time.
	;--------------------------------------------------------------------------
	if(settoview eq 200407192) then begin
		print, 'DISPLAYING JULY112004 SetB (AUTO LUNG SEGMENTATION) RESULTS with SETB truth.........'
		dir_image = 'G:\ChestCad\IPL_Processed_Tiff\SetA\'
		dir_hit = 'L:\chestCad\Development\DataSets\SetB\GroundTruth\'
		dir_can = 'G:\detectionalysis\Debug\2004\July112004\SetB\CanFiles\'
		fndet = 'G:\detectionalysis\Debug\2004\July112004\SetB\OutCan\GML_KNNrunoutput__classificationresults.txt'
	endif

	;--------------------------------------------------------------------------
	; Find the files in the specified directories.
	;--------------------------------------------------------------------------
	imagefiles = findfile(dir_image + image_extention)	; .dcm
	hitfiles = findfile(dir_hit + '*.hit')
	if(n_elements(dir_can) ne 0) then canfiles = findfile(dir_can + '*.can')

	print, 'There are ' + strtrim(string(n_elements(imagefiles)),2) + ' image files.'
	print, 'There are ' + strtrim(string(n_elements(hitfiles)),2) + ' hit files.'
	if(n_elements(dir_can) ne 0) then print, 'There are ' + strtrim(string(n_elements(canfiles)),2) + ' can files.'

	if(n_elements(reduce) eq 0) then downsamplefactor = 1.0 else downsamplefactor = reduce

	;--------------------------------------------------------------------------
	; Form a composite list of filenames that exclide the path and the suffix.
	;--------------------------------------------------------------------------
	trimmed_imagefiles = trimpathandsuffix(imagefiles)
	trimmed_hitfiles = trimpathandsuffix(hitfiles)
	if(n_elements(dir_can) ne 0) then trimmed_canfiles = trimpathandsuffix(canfiles)

	if(n_elements(dir_can) ne 0) then begin
		combinedfilelist = [trimmed_imagefiles, trimmed_hitfiles, trimmed_canfiles]
	endif else begin
		combinedfilelist = [trimmed_imagefiles, trimmed_hitfiles]
	endelse

	print, n_elements(combinedfilelist)
	sortedcombinedfilelist = combinedfilelist[sort(combinedfilelist)]
	uniquecombinedfilelist = sortedcombinedfilelist[uniq(sortedcombinedfilelist)]
	print, n_elements(uniquecombinedfilelist)
	print, transpose(uniquecombinedfilelist)


;	for i=0L,n_elements(uniquecombinedfilelist)-1 do begin

	i = 0
	while(i lt n_elements(uniquecombinedfilelist)) do begin

		image_index = where(trimmed_imagefiles eq uniquecombinedfilelist[i], count_image)
		hit_index = where(trimmed_hitfiles eq uniquecombinedfilelist[i], count_hit)
		if(n_elements(dir_can) ne 0) then begin
			can_index = where(trimmed_canfiles eq uniquecombinedfilelist[i], count_can)
		endif else begin
			count_can = 0
		endelse

		if(count_image ne 0) then begin

			fnimage = imagefiles[image_index]

			windowid1 = display_ccad_image(fnimage, inputdownsamplefactor=downsamplefactor, whichimage=1, displaymode=displaymode)

			if(count_hit ne 0) then fnhit = hitfiles[hit_index]
			if(count_can ne 0) then fncan = canfiles[can_index]

			if((count_hit eq 0) and (count_can eq 0)) then begin
				windowid2 = display_ccad_image(fnimage, detfilename=fndet, inputdownsamplefactor=downsamplefactor, whichimage=2, displaymode=displaymode)
			endif else begin
				if(count_hit eq 0) then begin
					windowid2 = display_ccad_image(fnimage, canfilename=fncan, detfilename=fndet, inputdownsamplefactor=downsamplefactor, whichimage=2,displaymode=displaymode)
				endif
				if(count_can eq 0) then begin
					windowid2 = display_ccad_image(fnimage, hitfilename=fnhit, detfilename=fndet, /displaytruthflag, inputdownsamplefactor=downsamplefactor,whichimage=2,displaymode=displaymode)
				endif
				if((count_hit ne 0) and (count_can ne 0)) then begin
					windowid2 = display_ccad_image(fnimage, hitfilename=fnhit, canfilename=fncan, detfilename=fndet, /displaytruthflag, inputdownsamplefactor=downsamplefactor,whichimage=2, displaymode=displaymode)
				endif
			endelse
		endif

		;----------------------------------------------------------------------
		; Get input from the user as to whether to continue or to quit.
		;----------------------------------------------------------------------
		inputnotvalid = 1
		while(inputnotvalid) do begin
			userinputvalue = ''
			read, userinputvalue, prompt='Enter (p)(n)(b)(x)(s)(t)(a)(q)'
			if(strcmp(userinputvalue, 'n') eq 1) then begin
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 'p') eq 1) then begin
               if(i ne 0) then begin
                  i = i - 2
			      inputnotvalid = 0
			   endif
			endif
			if(strcmp(userinputvalue, 'xxs') eq 1) then begin
               i = i - 1
               displaymode = 0
               downsamplefactor = 6
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 'xs') eq 1) then begin
               i = i - 1
               displaymode = 0
               downsamplefactor = 5
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 's') eq 1) then begin
               i = i - 1
               displaymode = 0
               downsamplefactor = 4
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 't') eq 1) then begin
               i = i - 1
               displaymode = 0
               downsamplefactor = 2
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 'a') eq 1) then begin
               i = i - 1
               displaymode = 1
               downsamplefactor = 1
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 'b') eq 1) then begin
               i = i - 1
               displaymode = 0
               downsamplefactor = 1
			   inputnotvalid = 0
			endif
			if(strcmp(userinputvalue, 'q') eq 1) then begin
				if(windowid1 ge 0) then wdelete, windowid1
				if(windowid2 ge 0) then wdelete, windowid2
				return
			endif
		endwhile

		if(windowid1 ge 0) then wdelete, windowid1
		if(windowid2 ge 0) then wdelete, windowid2

		i = i + 1

;	endfor

	endwhile
end
