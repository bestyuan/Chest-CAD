;===============================================================================
; Program: detection_io.pro
; Purpose: To provide function for reading detection files.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/25/2002
;===============================================================================

function read_heath_detection_file, filename, detections=detections

   compile_opt strictarr

   detection = {filename:ptr_new(), cols:0L, rows:0L, $
      value:ptr_new(), xcoord:ptr_new(), ycoord:ptr_new()}

   ;============================================================================
   ; Open the file for reading.
   ;============================================================================
   openr, detlun, filename, /get_lun

   ;============================================================================
   ; Read in all of the detections.
   ;============================================================================
   ts = strarr(1)
   while(not eof(detlun)) do begin
      readf, detlun, ts
      if(strcmp(ts[0], '#', strlen('#')) eq 1) then begin
      endif else break
   endwhile
   twovals = strsplit(ts, /extract)
   cols = long(twovals[0])
   rows = long(twovals[1])
   while(not eof(detlun)) do begin
      readf, detlun, ts
      twovals = strsplit(ts, /extract)
      if(n_elements(value) eq 0) then value = float(twovals[1]) else value = [value, float(twovals[1])]
      readf, detlun, ts
      twovals = strsplit(ts, /extract)
      if(n_elements(xcoord) eq 0) then xcoord = float(twovals[0]) else xcoord = [xcoord, float(twovals[0])]
      if(n_elements(ycoord) eq 0) then ycoord = float(twovals[1]) else ycoord = [ycoord, float(twovals[1])]
   endwhile

   detections = {filename:filename, cols:cols, rows:rows, value:value, xcoord:xcoord, ycoord:ycoord}

   free_lun, detlun

   return, 1
end

function read_senn_detection_file, filename, detections=detections

   compile_opt strictarr

   cols = -1
   rows = -1

   detection = {filename:ptr_new(), cols:0L, rows:0L, $
      value:ptr_new(), xcoord:ptr_new(), ycoord:ptr_new()}

   ;============================================================================
   ; Open the file for reading.
   ;============================================================================
   openr, detlun, filename, /get_lun

   ;============================================================================
   ; Read in all of the detections.
   ;============================================================================
   ts = strarr(1)
   ;while(not eof(detlun)) do begin
   ;   readf, detlun, ts
   ;   if(strcmp(ts[0], '#', strlen('#')) eq 1) then begin
   ;   endif else break
   ;endwhile
   ;twovals = strsplit(ts, /extract)
   ;cols = long(twovals[0])
   ;rows = long(twovals[1])
   while(not eof(detlun)) do begin
      readf, detlun, ts
      if(strlen(ts) eq 0) then continue
      threevals = strsplit(ts, /extract)
      if(n_elements(xcoord) eq 0) then xcoord = float(threevals[0]) else xcoord = [xcoord, float(threevals[0])]
      if(n_elements(ycoord) eq 0) then ycoord = float(threevals[1]) else ycoord = [ycoord, float(threevals[1])]
      if(n_elements(value) eq 0) then value = float(threevals[2]) else value = [value, float(threevals[2])]
   endwhile

   detections = {filename:filename, cols:cols, rows:rows, value:value, xcoord:xcoord, ycoord:ycoord}

   free_lun, detlun

   return, 1
end

function read_canv11_file, filename, detections=detections

	compile_opt strictarr


	cols = -1
	rows = -1

	detection = {filename:ptr_new(), cols:0L, rows:0L, $
		value:ptr_new(), xcoord:ptr_new(), ycoord:ptr_new()}

	;============================================================================
	; Open the file for reading.
	;============================================================================
	openr, detlun, filename, /get_lun

	;============================================================================
	; Read in all of the detections.
	;============================================================================
	ts = strarr(1)
	;while(not eof(detlun)) do begin
	;   readf, detlun, ts
	;   if(strcmp(ts[0], '#', strlen('#')) eq 1) then begin
	;   endif else break
	;endwhile
	;twovals = strsplit(ts, /extract)
	;cols = long(twovals[0])
	;rows = long(twovals[1])
	while(not eof(detlun)) do begin
		readf, detlun, ts
		if(strlen(ts) eq 0) then continue

		threevals = strsplit(ts, /extract)

		if(strcmp(threevals[0], "centroid:", strlen("centroid:")) eq 1) then begin
			if(n_elements(xcoord) eq 0) then xcoord = float(threevals[1]) else xcoord = [xcoord, float(threevals[1])]
			if(n_elements(ycoord) eq 0) then ycoord = float(threevals[2]) else ycoord = [ycoord, float(threevals[2])]
		endif

	endwhile

	value = 0 * xcoord
	detections = {filename:filename, cols:cols, rows:rows, value:value, xcoord:xcoord, ycoord:ycoord}

	free_lun, detlun

	return, 1

end

pro test_read_heath_detection_file

   fn = 'F:\ChestCAD\src\EK0002_0_PA_P_CR400_CO_afum.pgm.det'

   status = read_heath_detection_file(fn, detections=detections)
   print, status
   help, detections, /struct

   plot, detections.xcoord, detections.rows-detections.ycoord, psym=1, /device
end

pro test_read_senn_detection_file

   fn = 'L:\CHESTCAD\TESTS\TEST0\INTERMEDIATERESULTS\INITIALCANDIDATES\EK0004_0_PA_P_CR400_CO.cnl'

   status = read_senn_detection_file(fn, detections=detections)
   print, status
   detections.rows = 2500
   detections.cols = 2048
   help, detections, /struct

   plot, detections.xcoord, detections.rows-detections.ycoord, psym=1, /device
   print, transpose(detections.value)
end
