;===============================================================================
; Program: hit_io.pro
; Purpose: This file implements functions for reading and writing Health Imaging
; Truth (.hit) files in IDL. The functions here implement version 'V1'.
; These functions also read and write specific additional information stored
; in the comments of the file. These include references to source data and
; both intensity and geometric processing parameters.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================

;===============================================================================
; Procedure: free_hit_data
; Purpose: To free the pointers associated with the hit_data.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================
pro free_hit_data, hit_data

   compile_opt strictarr

   if(ptr_valid(hit_data.filename)) then ptr_free, hit_data.filename
   if(ptr_valid(hit_data.version)) then ptr_free, hit_data.version
   if(ptr_valid(hit_data.date_time_generated)) then ptr_free, hit_data.date_time_generated
   if(ptr_valid(hit_data.source_image)) then ptr_free, hit_data.source_image
   if(ptr_valid(hit_data.source_cad)) then ptr_free, hit_data.source_cad
   if(ptr_valid(hit_data.source_truth_type)) then ptr_free, hit_data.source_truth_type
   if(ptr_valid(hit_data.source_truth)) then ptr_free, hit_data.source_truth
   if(ptr_valid(hit_data.xraySourceID)) then ptr_free, hit_data.xraySourceID
   for i=0L,hit_data.numNodules-1 do begin
      if(ptr_valid((*hit_data.nodule)[i].status)) then $
         ptr_free, (*hit_data.nodule)[i].status
      if(ptr_valid((*hit_data.nodule)[i].xycoords)) then $
         ptr_free, (*hit_data.nodule)[i].xycoords
   endfor
end

;===============================================================================
; Function: generate_hit_data_variable
; Purpose: This function generates a variable of a structure type that contains
; placeholders for the information in a Health Imaging Truth (.hit) file.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================
function generate_hit_data_variable

   compile_opt strictarr

   hit_data = {filename:ptr_new(), $
      version:ptr_new(), $
      date_time_generated:ptr_new(), $
      source_image:ptr_new(), $
      image_rotate:0L, $
      image_flip:0L, $
      image_scalemin:0L, $
      image_scalemax:0L, $
      source_cad:ptr_new(), $
      cad_rotate:0L, $
      cad_flip:0L, $
      source_truth_type:ptr_new(), $
      source_truth:ptr_new(), $
      truth_rotate:0L, $
      truth_flip:0L, $
      xraySourceID:ptr_new(), $
      imageSize_cols:0L, $
      imageSize_rows:0L, $
      numNodules:0L, $
      nodule:ptr_new()}

   return, hit_data
end

;===============================================================================
; Function: generate_nodule_variable
; Purpose: To create a variable of a structure type that contains placeholders
; for the information about a nodule.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================
function generate_nodule_variable

   compile_opt strictarr

   nodule_data = {index:0L, status:ptr_new(), numpoints:0L, $
      centroidx:0L, centroidy:0L, xycoords:ptr_new()}

   return, nodule_data
end

;===============================================================================
; Procedure: read_hit_file
; Purpose: To read the information from a Health Imaging Truth (.hit) file
; into a data structure.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================
function read_hit_file, filename, hit_data=hit_data

   compile_opt strictarr

   hit_data = generate_hit_data_variable()

   ts = strarr(1)
   num_expected_points = 0L

   ;============================================================================
   ; Open the file for reading.
   ;============================================================================
   openr, luninp, filename, /get_lun
   if(luninp lt 0) then return, 0

   hit_data.filename = ptr_new(filename)

   ;============================================================================
   ; Check to see that this is actually a hit file.
   ;============================================================================
   readf, luninp, ts
   if(strcmp(ts[0], '#^HIT V1', strlen('#^HIT V1')) eq 0) then begin
      free_hit_data, hit_data
      return, 0
   endif else hit_data.version = ptr_new(ts[0])

   ;============================================================================
   ; Read in the rest of the data from the file.
   ;============================================================================
   while(not eof(luninp)) do begin
      readf, luninp, ts

      ;=========================================================================
      ; Comments may hold some data we want. If the line starts with a '#' then
      ; the line is a comment. In this case, search for types of comments that
      ; contain information of interest.
      ;=========================================================================
      if(strcmp(ts[0],'#',strlen('#')) eq 1) then begin
         ; print, '###' + ts[0]
         if(strcmp(ts[0],'# Date/Time Generated: ',strlen('# Date/Time Generated: ')) eq 1) then begin
            hit_data.date_time_generated = ptr_new(strmid(ts[0], strlen('# Date/Time Generated: '), $
               strlen(ts[0]) - strlen('# Date/Time Generated: ')))
            continue
         endif
         if(strcmp(ts[0],'# Source Image: ',strlen('# Source Image: ')) eq 1) then begin
            hit_data.source_image = ptr_new(strmid(ts[0], strlen('# Source Image: '), $
               strlen(ts[0]) - strlen('# Source Image: ')))
            continue
         endif
         if(strcmp(ts[0],'# Image: rotate ',strlen('# Image: rotate ')) eq 1) then begin
            hit_data.image_rotate = long(strmid(ts[0], strlen('# Image: rotate '), $
               strlen(ts[0]) - strlen('# Image: rotate ')))
            continue
         endif
         if(strcmp(ts[0],'# Image: flip',strlen('# Image: flip')) eq 1) then begin
            hit_data.image_flip = 1
            continue
         endif
         if(strcmp(ts[0],'# Image: scale ',strlen('# Image: scale ')) eq 1) then begin
            scalevals = strmid(ts[0], strlen('# Image: scale '), $
               strlen(ts[0]) - strlen('# Image: scale '))
            scalevals = strsplit(scalevals, /extract)
            hit_data.image_scalemin = long(scalevals[0])
            hit_data.image_scalemax = long(scalevals[1])
            continue
         endif
         if(strcmp(ts[0],'# Source CAD: ',strlen('# Source CAD: ')) eq 1) then begin
            hit_data.source_cad = ptr_new(strmid(ts[0], strlen('# Source CAD: '), $
               strlen(ts[0]) - strlen('# Source CAD: ')))
            continue
         endif
         if(strcmp(ts[0],'# CAD: rotate ',strlen('# CAD: rotate ')) eq 1) then begin
            hit_data.cad_rotate = long(strmid(ts[0], strlen('# CAD: rotate '), $
               strlen(ts[0]) - strlen('# CAD: rotate ')))
            continue
         endif
         if(strcmp(ts[0],'# CAD: flip',strlen('# CAD: flip')) eq 1) then begin
            hit_data.cad_flip = 1
            continue
         endif
         if(strcmp(ts[0],'# Source Truth_Type: ',strlen('# Source Truth_Type: ')) eq 1) then begin
            ; print, '# Source Truth_Type: ' + ts[0]
            hit_data.source_truth_type = ptr_new(strmid(ts[0], strlen('# Source Truth_Type: '), $
               strlen(ts[0]) - strlen('# Source Truth_Type: ')))
            continue
         endif
         if(strcmp(ts[0],'# Source Truth: ',strlen('# Source Truth: ')) eq 1) then begin
            ; print, '# Source Truth: ' + ts[0]
            if(ptr_valid(hit_data.source_truth)) then begin
               truth_list = *hit_data.source_truth
               ptr_free, hit_data.source_truth
               truth_list = [truth_list, strmid(ts[0], strlen('# Source Truth: '), $
                  strlen(ts[0]) - strlen('# Source Truth: '))]
               hit_data.source_truth = ptr_new(truth_list)
            endif else begin
               hit_data.source_truth = ptr_new(strmid(ts[0], strlen('# Source Truth: '), $
                  strlen(ts[0]) - strlen('# Source Truth: ')))
            endelse
            continue
         endif
         if(strcmp(ts[0],'# Truth: rotate ',strlen('# Truth: rotate ')) eq 1) then begin
            hit_data.truth_rotate = long(strmid(ts[0], strlen('# Truth: rotate '), $
               strlen(ts[0]) - strlen('# Truth: rotate ')))
            continue
         endif
         if(strcmp(ts[0],'# Truth: flip',strlen('# Truth: flip')) eq 1) then begin
            hit_data.truth_flip = 1
            continue
         endif
      endif

      ;=========================================================================
      ; When we do not have a comment and are not currently reading a list of
      ; points, the following code is used to read information from the file.
      ;=========================================================================
      if(num_expected_points eq 0) then begin
         if(strcmp(ts[0],'xraySourceID: ',strlen('xraySourceID: ')) eq 1) then begin
            hit_data.xraySourceID = ptr_new(strmid(ts[0], strlen('xraySourceID: '), $
               strlen(ts[0]) - strlen('xraySourceID: ')))
            continue
         endif
         if(strcmp(ts[0],'imageSize: ',strlen('imageSize: ')) eq 1) then begin
            sizevals = long(strsplit(strmid(ts[0], strlen('imageSize: '), $
               strlen(ts[0]) - strlen('imageSize: ')),' ', /extract))
            hit_data.imageSize_cols = sizevals[0]
            hit_data.imageSize_rows = sizevals[1]
            continue
         endif
         if(strcmp(ts[0],'numNodules: ',strlen('numNodules: ')) eq 1) then begin
            hit_data.numNodules = long(strmid(ts[0], strlen('numNodules: '), $
               strlen(ts[0]) - strlen('numNodules: ')))
            nodule_variable = generate_nodule_variable()
            hit_data.nodule = ptr_new(replicate(nodule_variable, hit_data.numNodules))
            nindex = -1
            continue
         endif
         if(strcmp(ts[0],'noduleIndex: ',strlen('noduleIndex: ')) eq 1) then begin
            nindex = nindex + 1
            nodule_index = long(strmid(ts[0], strlen('noduleIndex: '), $
               strlen(ts[0]) - strlen('noduleIndex: ')))
            (*hit_data.nodule)[nindex].index = nodule_index
            continue
         endif
         if(strcmp(ts[0],'status: ',strlen('status: ')) eq 1) then begin
            (*hit_data.nodule)[nindex].status = ptr_new(strmid(ts[0], strlen('status: '), $
               strlen(ts[0]) - strlen('status: ')))
            continue
         endif
         if(strcmp(ts[0],'centroid: ',strlen('centroid: ')) eq 1) then begin
            centroidvals = long(strsplit(strmid(ts[0], strlen('centroid: '), $
               strlen(ts[0]) - strlen('centroid: ')),' ', /extract))
            (*hit_data.nodule)[nindex].centroidx = centroidvals[0]
            (*hit_data.nodule)[nindex].centroidy = centroidvals[1]
            continue
         endif
         if(strcmp(ts[0],'nPoints: ',strlen('nPoints: ')) eq 1) then begin
            (*hit_data.nodule)[nindex].numpoints = long(strmid(ts[0], strlen('nPoints: '), $
               strlen(ts[0]) - strlen('nPoints: ')))
            (*hit_data.nodule)[nindex].xycoords = $
               ptr_new(lonarr(2,(*hit_data.nodule)[nindex].numpoints), /no_copy)
            num_expected_points = (*hit_data.nodule)[nindex].numpoints
            continue
         endif

      ;=========================================================================
      ; Once we start reading points, we must read nPoints of them before
      ; reading any other data from the file. The following section of code
      ; is used to read a point.
      ;=========================================================================
      endif else begin
         twocoords = long(strsplit(ts[0], ' ', /extract))
         (*(*hit_data.nodule)[nindex].xycoords)[0,(*hit_data.nodule)[nindex].numpoints - num_expected_points] = twocoords[0]
         (*(*hit_data.nodule)[nindex].xycoords)[1,(*hit_data.nodule)[nindex].numpoints - num_expected_points] = twocoords[1]
         num_expected_points = num_expected_points - 1
      endelse

   endwhile

   ;============================================================================
   ; Early .hit files did not contain a comment regarding the type of source
   ; truth file. If no such data was found, make an educated guess at it.
   ;============================================================================
   if(ptr_valid(hit_data.source_truth_type) eq 0) then begin
      if(ptr_valid(hit_data.source_truth) eq 1) then begin
         if(n_elements(*hit_data.source_truth) gt 1) then begin
            hit_data.source_truth_type = ptr_new('BOUNDARY')
         endif else begin
            if(strmatch(*hit_data.source_truth, '*.tru') eq 1) then $
               hit_data.source_truth_type = ptr_new('TRU')
            if(strmatch(*hit_data.source_truth, '*boundary_*') eq 1) then $
               hit_data.source_truth_type = ptr_new('BOUNDARY')
         endelse
      endif
   endif

   ; help, hit_data, /struct

   free_lun, luninp

   return, 1
end

;===============================================================================
; Function: write_hit_file
; Purpose: To create a Health Imaging Truth file.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================
function write_hit_file, hit_data

   compile_opt strictarr

   ;============================================================================
   ; Open the file for writing.
   ;============================================================================
   openw, lunout, *hit_data.filename, /get_lun
   if(lunout lt 0) then return, 0

   ;============================================================================
   ; Write the version. This is required for all .hit files.
   ;============================================================================
   printf, lunout, *hit_data.version

   ;============================================================================
   ; Write the date and time the file was generated.
   ;============================================================================
   date_time_generated = systime()
   if(ptr_valid(hit_data.date_time_generated)) then ptr_free, hit_data.date_time_generated
   hit_data.date_time_generated = ptr_new(date_time_generated, /no_copy)
   printf, lunout, '# Date/Time Generated: ', *hit_data.date_time_generated

   ;============================================================================
   ; Write all information about the source image.
   ;============================================================================
   if(ptr_valid(hit_data.source_image)) then begin
      printf, lunout, '# Source Image: ' + *hit_data.source_image
      if(hit_data.image_rotate ne 0L) then printf, lunout, '# Image: rotate ' + strtrim(string(hit_data.image_rotate),2)
      if(hit_data.image_flip ne 0L) then printf, lunout, '# Image: flip'
      if(not((hit_data.image_scalemin eq 0L) and (hit_data.image_scalemax eq 0L))) then begin
         printf, lunout, '# Image: scale ' + $
            strtrim(string(hit_data.image_scalemin),2) + ' ' + $
            strtrim(string(hit_data.image_scalemax),2)
      endif
   endif

   ;============================================================================
   ; Write all the information about the cad file.
   ;============================================================================
   if(ptr_valid(hit_data.source_cad)) then begin
      printf, lunout, '# Source CAD: '+ *hit_data.source_cad
      if(hit_data.cad_rotate ne 0L) then $
         printf, lunout, '# CAD: rotate ' + strtrim(string(hit_data.cad_rotate),2)
      if(hit_data.cad_flip ne 0L) then printf, lunout, '# CAD: flip'
   endif

   ;============================================================================
   ; Write all the information about the truth file.
   ;============================================================================
   if(ptr_valid(hit_data.source_truth_type)) then begin
      if((*hit_data.source_truth_type eq 'TRU') and ptr_valid(hit_data.source_truth)) then begin
         printf, lunout, '# Source Truth_Type: ' + *hit_data.source_truth_type
         printf, lunout, '# Source Truth: ' + *hit_data.source_truth
         ; This type of truth cannot be rotated or flipped!
         ; if(hit_data.truth_rotate ne 0L) then $
         ;   printf, lunout, '# Truth: rotate ' + strtrim(string(hit_data.truth_rotate),2)
         ; if(hit_data.truth_flip ne 0L) then printf, lunout, '# Truth: flip'
      endif
      if((*hit_data.source_truth_type eq 'BOUNDARY') and ptr_valid(hit_data.source_truth)) then begin
         printf, lunout, '# Source Truth_Type: ' + *hit_data.source_truth_type
         for i=0L,n_elements(*hit_data.source_truth)-1 do begin
            printf, lunout, '# Source Truth: ' + *hit_data.source_truth[i]
         endfor
         if(hit_data.truth_rotate ne 0L) then $
            printf, lunout, '# Truth: rotate ' + strtrim(string(hit_data.truth_rotate),2)
         if(hit_data.truth_flip ne 0L) then printf, lunout, '# Truth: flip'
      endif
   endif

   ;============================================================================
   ; Write the required image name and dimensions and the number of nodules.
   ;============================================================================
   printf, lunout, 'xraySourceID: ' + *hit_data.xraySourceID
   printf, lunout, 'imageSize: ', strtrim(string(hit_data.imageSize_cols),2) + ' ' + $
      strtrim(string(hit_data.imageSize_rows),2)
   printf, lunout, 'numNodules: ' + strtrim(string(hit_data.numNodules),2)

   ;============================================================================
   ; Finally, write out the information for each nodule.
   ;============================================================================
   for i=0L,hit_data.numNodules-1 do begin
      printf, lunout, 'noduleIndex: ', strtrim(string((*hit_data.nodule)[i].index),2)
      printf, lunout, 'status: ', *(*hit_data.nodule)[i].status
      printf, lunout, 'centroid: ' + $
         strtrim(string((*hit_data.nodule)[i].centroidx),2) + ' ' + $
         strtrim(string((*hit_data.nodule)[i].centroidy),2)
      printf, lunout, 'nPoints: ' + strtrim(string((*hit_data.nodule)[i].numpoints),2)
      for j=0L,(*hit_data.nodule)[i].numpoints-1 do begin
         printf, lunout, strtrim(string((*(*hit_data.nodule)[i].xycoords)[0,j]),2) + $
            ' ' + strtrim(string((*(*hit_data.nodule)[i].xycoords)[1,j]),2)
      endfor
   endfor

   free_lun, lunout

   return, 1
end

pro test_read_write_hit_file

  compile_opt strictarr

  ;fn = 'D:\l465657\ChestCAD\NYH_Test0\Truth\EK0004_0_PA_P_CR400_CO.hit'
  fn = 'D:\l465657\ChestCAD\NYH_Test0\Truth\EK0016_0_PA_P_CR400_CO.hit'

  status = read_hit_file(fn, hit_data=hit_data)

  if(status ne 1) then return

  fnout = 'D:\l465657\ChestCAD\SampleHitFile.hit'
  ptr_free, hit_data.filename
  hit_data.filename = ptr_new(fnout)

  status = write_hit_file(hit_data)
  if(status eq 1) then print, 'Successfull!!!'

end