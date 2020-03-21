;===============================================================================
; Program: can_io.pro
; Purpose: This file implements functions for reading candidate files in IDL.
; The functions here implement version 'V1.1'.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/20/2003
;===============================================================================

;===============================================================================
; Procedure: free_can_data
; Purpose: To free the pointers associated with the can_data.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/20/2003
;===============================================================================
pro free_can_data, can_data

   compile_opt strictarr

   if(ptr_valid(can_data.filename)) then ptr_free, can_data.filename
   if(ptr_valid(can_data.version)) then ptr_free, can_data.version
   if(ptr_valid(can_data.xraySourceID)) then ptr_free, can_data.xraySourceID
   if(ptr_valid(can_data.flabels)) then ptr_free, can_data.flabels
   for i=0L,can_data.numCandidates-1 do begin
      if(ptr_valid((*can_data.candidate)[i].xycoords)) then $
         ptr_free, (*can_data.candidate)[i].xycoords
      if(ptr_valid((*can_data.candidate)[i].groups)) then $
         ptr_free, (*can_data.candidate)[i].groups
      if(ptr_valid((*can_data.candidate)[i].groundtruth)) then $
         ptr_free, (*can_data.candidate)[i].groundtruth
      if(ptr_valid((*can_data.candidate)[i].ftrVector)) then $
         ptr_free, (*can_data.candidate)[i].ftrVector
      if(ptr_valid((*can_data.candidate)[i].probability)) then $
         ptr_free, (*can_data.candidate)[i].probability
   endfor
   can_data.numCandidates = 0
   if(ptr_valid(can_data.candidate)) then ptr_free, can_data.candidate
end

;===============================================================================
; Function: generate_can_data_variable
; Purpose: This function generates a variable of a structure type that contains
; placeholders for the information in a candidate file.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/20/2003
;===============================================================================
function generate_can_data_variable

   compile_opt strictarr

   can_data = {filename:ptr_new(), $
      version:ptr_new(), $
      xraySourceID:ptr_new(), $
      imageSize_cols:0L, $
      imageSize_rows:0L, $
      numCandidates:0L, $
      typeRegion:'n', $		; n means not defined, m = binary bask, b = boundary data
      numFeatures:0L, $
      flabels:ptr_new(), $
      candidate:ptr_new()}
   return, can_data
end

;===============================================================================
; Function: generate_candidate_variable
; Purpose: To create a variable of a structure type that contains placeholders
; for the information about a candidate nodule.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/20/2003
;===============================================================================
function generate_candidate_variable

	compile_opt strictarr

	candidate_data = {index:0L, ngroups:0L, groups:ptr_new(), $
	groundtruth:ptr_new(), probability:ptr_new(), numpoints:0L, $
		ftrVector:ptr_new(), centroidx:0L, centroidy:0L, xycoords:ptr_new()}

	return, candidate_data
end

;===============================================================================
; Procedure: read_can_file_v11
; Purpose: To read the information from a candidate file into a data structure.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/20/2003
;function read_can_v11_file, filename=filename, candata=candata


;#^CAN V1.1
;xraySourceID: EK0002_0_PA_P_CR400_CO.img
;imageSize: 2048 2500
;numCandidates: 70
;typeRegion: b
;numFeatures: 35
;flabels: CoinNorm ***************
;endHeader:
;candidateIndex: 0
;groundtruth: unknown
;probability: 0
;ftrVector: 9.07916 *******************
;centroid: 565 485
;nPoints: 206
;584 435
;endCandidate:

;===============================================================================
function read_can_file_v11, filename, can_data=can_data

   compile_opt strictarr

   can_data = generate_can_data_variable()

   ts = strarr(1)
   num_expected_points = 0L

   ;============================================================================
   ; Open the file for reading.
   ;============================================================================
   openr, luninp, filename, /get_lun
   if(luninp lt 0) then return, 0

   can_data.filename = ptr_new(filename)

   ;============================================================================
   ; Check to see that this is actually a CAN file.
   ;============================================================================
   readf, luninp, ts
   if((strcmp(ts[0], '#^CAN V1.2', strlen('#^CAN V1.2')) eq 0) and (strcmp(ts[0], '#^CAN V1.2', strlen('#^CAN V1.2')) eq 0)) then begin
      free_can_data, can_data
      return, 0
   endif else can_data.version = ptr_new(ts[0])

   ;============================================================================
   ; Read in the rest of the data from the file.
   ;============================================================================
   while(not eof(luninp)) do begin
      readf, luninp, ts

      ;=========================================================================
      ; When we do not have a comment and are not currently reading a list of
      ; points, the following code is used to read information from the file.
      ;=========================================================================
      if(num_expected_points eq 0) then begin
         if(strcmp(ts[0],'xraySourceID: ',strlen('xraySourceID: ')) eq 1) then begin
            can_data.xraySourceID = ptr_new(strmid(ts[0], strlen('xraySourceID: '), $
               strlen(ts[0]) - strlen('xraySourceID: ')))
            continue
         endif
         if(strcmp(ts[0],'imageSize: ',strlen('imageSize: ')) eq 1) then begin
            sizevals = long(strsplit(strmid(ts[0], strlen('imageSize: '), $
               strlen(ts[0]) - strlen('imageSize: ')),' ', /extract))
            can_data.imageSize_cols = sizevals[0]
            can_data.imageSize_rows = sizevals[1]
            continue
         endif
         if(strcmp(ts[0],'numCandidates: ',strlen('numCandidates: ')) eq 1) then begin
            can_data.numCandidates = long(strmid(ts[0], strlen('numCandidates: '), $
               strlen(ts[0]) - strlen('numCandidates: ')))
            candidate_variable = generate_candidate_variable()
            can_data.candidate = ptr_new(replicate(candidate_variable, can_data.numCandidates))
            cindex = -1
            continue
         endif
         if(strcmp(ts[0],'typeRegion: ',strlen('typeRegion: ')) eq 1) then begin
			can_data.typeRegion = strtrim(string(strmid(ts[0], strlen('typeRegion: '), $
				strlen(ts[0]) - strlen('typeRegion: '))),2)
            continue
         endif
         if(strcmp(ts[0],'numFeatures: ',strlen('numFeatures: ')) eq 1) then begin
			can_data.numFeatures = long(strmid(ts[0], strlen('numFeatures: '), $
				strlen(ts[0]) - strlen('numFeatures: ')))
            continue
         endif
         if(strcmp(ts[0],'flabels: ',strlen('flabels: ')) eq 1) then begin
			can_data.flabels = ptr_new(strmid(ts[0], strlen('flabels: '), $
				strlen(ts[0]) - strlen('flabels: ')))
            continue
         endif
         if(strcmp(ts[0],'candidateIndex: ',strlen('candidateIndex: ')) eq 1) then begin
            candidate_index = long(strmid(ts[0], strlen('candidateIndex: '), $
               strlen(ts[0]) - strlen('candidateIndex: ')))
            cindex = cindex + 1
            (*can_data.candidate)[cindex].index = candidate_index

            continue
         endif
         if(strcmp(ts[0],'nGroups: ',strlen('nGroups: ')) eq 1) then begin
            (*can_data.candidate)[cindex].ngroups = long(strmid(ts[0], strlen('nGroups: '), $
               strlen(ts[0]) - strlen('nGroups: ')))
            continue
         endif
         if(strcmp(ts[0],'groups: ',strlen('groups: ')) eq 1) then begin
			(*can_data.candidate)[cindex].groups = ptr_new(strmid(ts[0], strlen('groups: '), $
				strlen(ts[0]) - strlen('groups: ')))
            continue
         endif
         if(strcmp(ts[0],'groundtruth: ',strlen('groundtruth: ')) eq 1) then begin
			(*can_data.candidate)[cindex].groundtruth = ptr_new(strmid(ts[0], strlen('groundtruth: '), $
				strlen(ts[0]) - strlen('groundtruth: ')))
            continue
         endif
         if(strcmp(ts[0],'probability: ',strlen('probability: ')) eq 1) then begin
			(*can_data.candidate)[cindex].probability = ptr_new(strmid(ts[0], strlen('probability: '), $
				strlen(ts[0]) - strlen('probability: ')))
            continue
         endif
         if(strcmp(ts[0],'ftrVector: ',strlen('ftrVector: ')) eq 1) then begin
			(*can_data.candidate)[cindex].ftrVector = ptr_new(strmid(ts[0], strlen('ftrVector: '), $
				strlen(ts[0]) - strlen('ftrVector: ')))
            continue
         endif
         if(strcmp(ts[0],'centroid: ',strlen('centroid: ')) eq 1) then begin
            centroidvals = long(strsplit(strmid(ts[0], strlen('centroid: '), $
               strlen(ts[0]) - strlen('centroid: ')),' ', /extract))
            (*can_data.candidate)[cindex].centroidx = centroidvals[0]
            (*can_data.candidate)[cindex].centroidy = centroidvals[1]
            continue
         endif
         if(strcmp(ts[0],'nPoints: ',strlen('nPoints: ')) eq 1) then begin
            (*can_data.candidate)[cindex].numpoints = long(strmid(ts[0], strlen('nPoints: '), $
               strlen(ts[0]) - strlen('nPoints: ')))
            ;print, (*can_data.candidate)[cindex].numpoints
            if((*can_data.candidate)[cindex].numpoints ne 0) then begin
	            (*can_data.candidate)[cindex].xycoords = $
   	            ptr_new(lonarr(2,(*can_data.candidate)[cindex].numpoints), /no_copy)
   	        endif
            num_expected_points = (*can_data.candidate)[cindex].numpoints
            continue
         endif

      ;=========================================================================
      ; Once we start reading points, we must read nPoints of them before
      ; reading any other data from the file. The following section of code
      ; is used to read a point.
      ;=========================================================================
      endif else begin
         twocoords = long(strsplit(ts[0], ' ', /extract))
         (*(*can_data.candidate)[cindex].xycoords)[0,(*can_data.candidate)[cindex].numpoints - num_expected_points] = twocoords[0]
         (*(*can_data.candidate)[cindex].xycoords)[1,(*can_data.candidate)[cindex].numpoints - num_expected_points] = twocoords[1]
         num_expected_points = num_expected_points - 1
      endelse

   endwhile

   free_lun, luninp

   return, 1
end

;=============================================================================
; Function: write_can_file
; Purpose: To write data to a CAN file.
; Name: Michael Heath, Eastman Kodak Company
; Date: 03/01/2004
;=============================================================================
function write_can_file, filename, can_data=can_data, newcomments=newcomments

	compile_opt strictarr

	v = -1
	if(strcmp(*can_data.version, '#^CAN V1.2', strlen('#^CAN V1.2')) eq 1) then begin
		v = 2
	endif else begin
		if(strcmp(*can_data.version, '#^CAN V1.1', strlen('#^CAN V1.1')) eq 1) then begin
			v = 1
		endif else begin
			if(strcmp(*can_data.version, '#^CAN V1', strlen('#^CAN V1')) eq 1) then v = 0
		endelse
	endelse

	if(v eq (-1)) then begin
		return, 0
	endif

	;print, v

	openw, lunout, filename, /get_lun
	printf, lunout, *can_data.version

	; Print out any new comments
	if(n_elements(newcomments) ne 0) then begin
		for c=0,n_elements(newcomments) -1 do begin
			printf, lunout, '# ' + newcomments[c]
		endfor
	endif

	printf, lunout, 'xraySourceID: ' + *can_data.xraySourceID
	printf, lunout, 'imageSize: ' + strtrim(string(can_data.imageSize_cols),2) + $
		' ' + strtrim(string(can_data.imageSize_rows),2)
	printf, lunout, 'numCandidates: ' + strtrim(string(can_data.numCandidates),2)
	printf, lunout, 'typeRegion: ' + can_data.typeRegion
	printf, lunout, 'numFeatures: ' + strtrim(string(can_data.numFeatures), 2)
	printf, lunout, 'flabels: ' + *can_data.flabels
	if(v ge 1) then printf, lunout, 'endHeader: '
	for i=0L, can_data.numCandidates - 1 do begin
		printf, lunout, 'candidateIndex: ' + strtrim(string((*can_data.candidate)[i].index),2)
		if(v eq 2) then begin
			printf, lunout, 'nGroups: ' + strtrim(string((*can_data.candidate)[i].ngroups),2)
			if((*can_data.candidate)[i].ngroups gt 0) then begin
				printf, lunout, 'groups: ' + *(*can_data.candidate)[i].groups
			endif
		endif
		if(v ge 1) then begin
			printf, lunout, 'groundtruth: ' + *(*can_data.candidate)[i].groundtruth
			printf, lunout, 'probability: ' + *(*can_data.candidate)[i].probability
		endif
		printf, lunout, 'ftrVector: ' + *(*can_data.candidate)[i].ftrVector
		printf, lunout, 'centroid: ' + strtrim(string((*can_data.candidate)[i].centroidx),2) + $
			' ' + strtrim(string((*can_data.candidate)[i].centroidy),2)
		printf, lunout, 'nPoints: ' + strtrim(string((*can_data.candidate)[i].numpoints),2)
		if((*can_data.candidate)[i].numpoints gt 0) then begin
			for p=0L, (*can_data.candidate)[i].numpoints -1 do begin
				printf, lunout, strtrim(string((*(*can_data.candidate)[i].xycoords)[0,p]),2) + $
					' ' + strtrim(string((*(*can_data.candidate)[i].xycoords)[1,p]),2)
			endfor
		endif
		if(v ge 1) then printf, lunout, 'endCandidate:'
	endfor

	free_lun, lunout
	return, 1
end