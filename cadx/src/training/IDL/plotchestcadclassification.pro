;==============================================================================
; Function: identify_zero_area_candidates
; Purpoe: We think that zero area candidates should be removed from the
; data prior to classification. THis function identifies which candidates
; have zero area.
; Name: Michael Heath, Eastman Kodak Company
; Date: 3/25/2004
;==============================================================================
function identify_zero_area_candidates, featurelabels, feature, area_featurelabel

    area_index = where(featurelabels eq area_featurelabel, count)

    if(count ne 1) then begin
       print, 'Error! The area feature was not found!'
       retall
    endif

    the_areas = feature[area_index, *]

    indx = where(the_areas eq 0.0, count)

    if(count eq 0) then return, [-1] else return, indx

end

;==============================================================================
; Procedure: remove_zero_area_candidates
; Purpose: We think that zero area candidates should be removed from the
; data prior to classification. This function removes those candidates from
; the feature set an from all data associated with candidates.
; Name: Michael Heath, Eastman Kodak Company
; Date: 3/25/2004
;
; Note: The current area_featurelabel on 3/25/2004 is "Shape.areaFraction"
;
;==============================================================================
pro remove_zero_area_candidates, in_featurelabels, in_feature, in_label, $
    in_candidate, in_imagefilename, in_correspondence, in_ngroups, in_groups, area_featurelabel, $
    out_featurelabels, out_feature, out_label, $
    out_candidate, out_imagefilename, out_correspondence, out_ngroups, out_groups, zero_candidate_indices

    ;==========================================================================
    ; Find the indices of zero area targets.
    ;==========================================================================
    indices = identify_zero_area_candidates(in_featurelabels, in_feature, area_featurelabel)

    ;==========================================================================
    ; Derive the nonzero area indices.
    ;==========================================================================
    if(indices[0] eq -1) then begin
       zero_candidate_indices = [-1]
       nonzero_indices = indgen(n_elements(in_label))
    endif else begin
    zero_candidate_indices = indices
       n = intarr(n_elements(in_label)) + 1
       n[indices] = 0
       nonzero_indices = where(n eq 1, count)
       if(count eq 0) then begin
         print, 'All areas are zero! No candidates remain! Returning!'
         retall
       endif
    endelse

    out_featurelabels = in_featurelabels
    out_feature = in_feature[*,nonzero_indices]
    out_label = in_label[nonzero_indices]
    out_candidate = in_candidate[nonzero_indices]
    out_imagefilename = in_imagefilename[nonzero_indices]
    out_correspondence = in_correspondence[*, nonzero_indices]
    out_ngroups = in_ngroups[nonzero_indices]
    out_groups = in_groups[*, nonzero_indices]

end


;==============================================================================
; Procedure: translate_groups
; Purpose: This procedure processes an an that lists the groups to which each
; candidate belongs and produces an array of groups that each contains a list
; of candidates an that group. Remember that the candidate indices do not
; represent candidate identifiers, but instead represent indices into an array
; of candidates.
; Name: Michael Heath
; Eastman Kodak Company
; Date: 01/31/2004
;==============================================================================
pro translate_groups, ngroups, groups, groupsize, groupmembers

    numcan = n_elements(ngroups)
    maxgroupid = max(groups)

    groupsize = intarr(maxgroupid+1)

    ; Determine the number of candidates
    ; in the largest group so we can properly
    ; size a the matrix that will list the candidate
    ; indices for each group.
    for i=0L,numcan-1 do begin
       for k=0L,ngroups[i]-1 do begin
          thisgroup = groups[k,i]
          groupsize[thisgroup] = $
             groupsize[thisgroup] + 1
       endfor
    endfor

    groupmembers =intarr(max(groupsize), maxgroupid+1)

    groupsize = groupsize * 0
    for i=0L,numcan-1 do begin
       for k=0L,ngroups[i]-1 do begin
          thisgroup = groups[k,i]
          groupmembers[groupsize[thisgroup], thisgroup] = i
          groupsize[thisgroup] = groupsize[thisgroup] + 1
       endfor
    endfor
end

; procedure cull_candidates_using_group, $
;    p_t, p_f, n, groupsize, groupmembers
;
;    1 2 3 4 5 6 7 --> n
;0: 0 1 1 0 0 0 0              0: 0 1 2 2 2 2 2
;1: 0 0 0 1 0 0 1              1: 0 0 0 1 1 1 2
;2: 1 1 0 1 0 1 1              2: 1 2 2 3 3 4 5
;3: 1 1 1 1 1 0 1              3: 1 2 2 4 5 5 6
;4: 0 0 0 0 1 1 1              4: 0 0 0 0 1 2 3

;o we use the number of neighbors that it took
;to obtain the threshold number of true nodules
;to declare a candidate as a nodule, or do we
;use the fraction of neighbors that are nodules?
;
; think that we should use the former, because
;it seems to best represent the idea behind the
;modified KNN classifier.
;
;This basically represents the apriori probaability
; at which the candidate is labeled a nodule by
; the modified KNN classifier.
;
;This approach quantizes the apriori probabilities
; in a was that could provide a consistent
; interface for both the modified KNN and GML
; classifiers.
;
; end

;==============================================================================
; Function: get_transplated_source_groups
; Purpose: This function extracts the candidate groups for a particular image
; by its filename. A list of candidates for all image has several entries
; in multiple arrays. The array imagefilename is simply a list of the filenames
; for images in which the candidate resides. The ngroups array is an array
; that specifies how many groups the the candidate belongs to. The matrix
; groups actually lists the group id's to which the candidate belongs. The
; ngroups[somecandidate] list how many columns of the groups array contain
; data for "somecandidate" in the matrix of groups. The string "thisfilename"
; specifies the image for which group information is desired. On successful
; output, the array groupsize is populated with the number of candidates in
; each group. The matrix groupmembers, lists the candidates that are in each
; group (listed in the columns for some particule groups (row)). A one is
; returned by the function on success and a zero is returned on failure. The
; candidate id's are the indices of candidates. The nodule id's of these
; candidates can be found by consulting the candidatelut.
; Name: Michael Heath, Eastman Kodak Company
; Date: 2/2/2004
;==============================================================================
function get_translated_source_groups, thisfilename, imagefilename, $
   candidate, ngroups, groups, groupsize, groupmembers

    indx = where(imagefilename eq thisfilename, count)
    if(count eq 0) then begin
       groupsize = 0
       groupmambers = 0
       return, 0
    endif

    relevent_ngroups = ngroups[indx]
    relevent_groups = groups[*,indx]
    noduleids = candidate[indx]

    translate_groups, relevent_ngroups, relevent_groups, groupsize, groupmembers

    ; Translate the nodule numbers to nodule id's.
    for i=0L, n_elements(groupsize)-1 do begin
       for j=0L, groupsize[i]-1 do begin
         groupmembers[j,i] = noduleids[groupmembers[j,i]]
       endfor
    endfor

    return, 1

end

;------------------------------------------------------------------------------
; Procedure: read_trn_v10
; Purpose: To read a chest cad training file. The format is very similar to
; the FRT training file used by the MATLAB code written for classification
; by Joe Revelli. This inherrited file format also includes the nodule ID
; of every truth region and the image each came from. This format contains
; all of the necessary information for algorithm training, feature selection
; and performance estimation.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;------------------------------------------------------------------------------
pro read_trn_v10, filename, featurelabels, feature, label, candidate, imagefilename, correspondence, $
    truthimagefilename, truthnoduleid

    ;#^TRN V1.0
    ;title: none
    ;nclasses: 2
    ;clabels: P N
    ;nsamples: 124 3166
    ;apriori: 0.500000 0.500000
    ;nfeatures: 35
    ;flabels: CoinNorm afum rdens grad vloc hloc lvar LocalVar11 LocalVar21 GMVar11 GMVar21 YDiff
    ;PtGrad WinGrad ProSymMean ProSymVar NormYPos GProSymMean GProSymVar Shape.areaFraction Shape.aspectRatio
    ;Shape.fit Shape.position Shape.areaToHullRatio Image.avgCodeValue Image.sigmaCodeValue Image.minCodeValue
    ;Image.maxCodeValue Dif.avgCodeValue Dif.sigmaCodeValue Dif.minCodeValue Dif.maxCodeValue Gradient.dirCoherence
    ;Gradient.nonuniformity Gradient.quality
    ;9.07916 85 0.0129148 108.812 0.0392157 0.0527578 0.0215238 0.249316 0.476489 0.796511 0.830301
    ;144.45 265 251.111 0.748532 3212.89 0.102757 0.968248 7696.69 0.00930405 2.53712 0.818111 0.985772
    ;0.80336 1430.9 76.545 1238 1741 108.905 32.2487 23 192 0.374789 0.1031 3.63519 N
    ;EK0002_0_PA_P_CR400_CO.img canID:_0
    ;numgroundtruth: 61
    ;EK0002_0_PA_P_CR400_CO.img 0

    line = strarr(1)

    openr, lun, filename, /get_lun
    if(lun lt 0) then return

    readf, lun, line
    if(strcmp(line[0], '#^TRN V1.0', strlen('#^TRN V1.0')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'title:', strlen('title:')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'nclasses:', strlen('nclasses:')) ne 1) then return
    if(strcmp(stringarray[1], '2', strlen('2')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'clabels:', strlen('clabels:')) ne 1) then return
    if(strcmp(stringarray[1], 'P', strlen('P')) ne 1) then return
    if(strcmp(stringarray[2], 'N', strlen('N')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'nsamples:', strlen('nsamples:')) ne 1) then return
    nsamples = long(stringarray[1]) + long(stringarray[2])

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'apriori:', strlen('apriori:')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'nfeatures:', strlen('nfeatures:')) ne 1) then return
    numfeatures = long(stringarray[1])

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'flabels:', strlen('flabels:')) ne 1) then return
    featurelabels = stringarray[1:numfeatures]

    ; print, featurelabels

    imagefilename = strarr(nsamples)
    candidate = intarr(nsamples)
    label = bytarr(nsamples)
    feature = dblarr(numfeatures, nsamples)
    correspondence = intarr(5, nsamples)

    for i=0L,(nsamples-1) do begin

       readf, lun, line
       stringarray = strsplit(line, ' ', /extract)
       for k=0L,(numfeatures-1) do begin
         feature[k,i] = double(stringarray[k])
       endfor
       tempchararray = byte(stringarray[numfeatures])
       label[i] = tempchararray[0]
       imagefilename[i] = stringarray[numfeatures+1]
       stringarray2 = strsplit(stringarray[numfeatures+2], '_', /extract)
       candidate[i] = fix(stringarray2[1])
       if(n_elements(stringarray2) gt 3) then begin
         ;print, 'Correspondence is present'
         if(strcmp(stringarray2[2], 'nodID:', strlen('nodID:')) eq 1) then begin
          correspondence[0,i] = n_elements(stringarray2)-3;
          for k=0L,correspondence[0,i]-1 do begin
              correspondence[k+1,i] = fix(stringarray2[3+k])
          endfor
         endif
       endif

       ;if((i lt 10) or (correspondence[0,i] gt 0)) then begin
       ; if(correspondence[0,i] ne 0) then begin
       ;     print, strtrim(string(label[i]),2) + ' ' + imagefilename[i] + ' ' + strtrim(string(candidate[i]),2) + $
       ;        ' '  + string(correspondence[1:correspondence[0,i],i])
       ; endif else begin
       ;     print, strtrim(string(label[i]),2) + ' ' + imagefilename[i] + ' ' +  strtrim(string(candidate[i]),2)
       ; endelse
       ;endif

    endfor

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'numgroundtruth:', strlen('numgroundtruth:')) ne 1) then return
    numgroundtruth = fix(stringarray[1])

    for i=0L,numgroundtruth-1 do begin
       readf, lun, line
       stringarray = strsplit(line, ' ', /extract)
       if(i eq 0) then begin
         truthimagefilename = stringarray[0]
         truthnoduleid = fix(stringarray[1])
       endif else begin
         truthimagefilename = [truthimagefilename, stringarray[0]]
         truthnoduleid = [truthnoduleid, fix(stringarray[1])]
       endelse

    endfor

    free_lun, lun
end

;------------------------------------------------------------------------------
; Procedure: read_trn_v11
; Purpose: To read a chest cad training file. The format is very similar to
; the FRT training file used by the MATLAB code written for classification
; by Joe Revelli. This inherrited file format also includes the nodule ID
; of every truth region and the image each came from. This format contains
; all of the necessary information for algorithm training, feature selection
; and performance estimation.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;------------------------------------------------------------------------------
pro read_trn_v11, filename, featurelabels, feature, label, candidate, imagefilename, correspondence, $
    truthimagefilename, truthnoduleid, ngroups, groups

    ;#^TRN V1.1
    ;title: none
    ;nclasses: 2
    ;clabels: P N
    ;nsamples: 124 3166
    ;apriori: 0.500000 0.500000
    ;nfeatures: 35
    ;flabels: CoinNorm afum rdens grad vloc hloc lvar LocalVar11 LocalVar21 GMVar11 GMVar21 YDiff
    ;PtGrad WinGrad ProSymMean ProSymVar NormYPos GProSymMean GProSymVar Shape.areaFraction Shape.aspectRatio
    ;Shape.fit Shape.position Shape.areaToHullRatio Image.avgCodeValue Image.sigmaCodeValue Image.minCodeValue
    ;Image.maxCodeValue Dif.avgCodeValue Dif.sigmaCodeValue Dif.minCodeValue Dif.maxCodeValue Gradient.dirCoherence
    ;Gradient.nonuniformity Gradient.quality
    ;9.07916 85 0.0129148 108.812 0.0392157 0.0527578 0.0215238 0.249316 0.476489 0.796511 0.830301
    ;144.45 265 251.111 0.748532 3212.89 0.102757 0.968248 7696.69 0.00930405 2.53712 0.818111 0.985772
    ;0.80336 1430.9 76.545 1238 1741 108.905 32.2487 23 192 0.374789 0.1031 3.63519 N
    ;EK0002_0_PA_P_CR400_CO.img canID:_0 Groups:_0
    ;numgroundtruth: 61
    ;EK0002_0_PA_P_CR400_CO.img 0

    maxgroups = 10   ; No nodule can belong to more than this many groups

    line = strarr(1)

    openr, lun, filename, /get_lun
    if(lun lt 0) then return

    readf, lun, line
    if(strcmp(line[0], '#^TRN V1.1', strlen('#^TRN V1.1')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'title:', strlen('title:')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'nclasses:', strlen('nclasses:')) ne 1) then return
    if(strcmp(stringarray[1], '2', strlen('2')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'clabels:', strlen('clabels:')) ne 1) then return
    if(strcmp(stringarray[1], 'P', strlen('P')) ne 1) then return
    if(strcmp(stringarray[2], 'N', strlen('N')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'nsamples:', strlen('nsamples:')) ne 1) then return
    nsamples = long(stringarray[1]) + long(stringarray[2])

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'apriori:', strlen('apriori:')) ne 1) then return

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'nfeatures:', strlen('nfeatures:')) ne 1) then return
    numfeatures = long(stringarray[1])

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'flabels:', strlen('flabels:')) ne 1) then return
    featurelabels = stringarray[1:numfeatures]

    ; print, featurelabels

    imagefilename = strarr(nsamples)
    candidate = intarr(nsamples)
    label = bytarr(nsamples)
    feature = dblarr(numfeatures, nsamples)
    correspondence = intarr(5, nsamples)
    ngroups = intarr(nsamples)
    groups = intarr(maxgroups,nsamples)

    for i=0L,(nsamples-1) do begin

       readf, lun, line
       stringarray = strsplit(line, ' ', /extract)
       for k=0L,(numfeatures-1) do begin
         feature[k,i] = double(stringarray[k])
       endfor
       tempchararray = byte(stringarray[numfeatures])
       label[i] = tempchararray[0]
       imagefilename[i] = stringarray[numfeatures+1]

       stringarray2 = strsplit(stringarray[numfeatures+2], '_', /extract)
       candidate[i] = fix(stringarray2[1])
       if(n_elements(stringarray2) gt 3) then begin
         ;print, 'Correspondence is present'
         if(strcmp(stringarray2[2], 'nodID:', strlen('nodID:')) eq 1) then begin
          correspondence[0,i] = n_elements(stringarray2)-3;
          for k=0L,correspondence[0,i]-1 do begin
              correspondence[k+1,i] = fix(stringarray2[3+k])
          endfor
         endif
       endif

        stringarray3 = strsplit(stringarray[numfeatures+3], '_', /extract)
       ngroups[i] = fix(n_elements(stringarray3)-1)
       groups[0,i] = fix(stringarray3[1])
       for r=1L,ngroups[i]-1 do begin
         groups[r,i] = fix(stringarray3[1+r])
       endfor

       ;if((i lt 10) or (correspondence[0,i] gt 0)) then begin
       ; if(correspondence[0,i] ne 0) then begin
       ;     print, strtrim(string(label[i]),2) + ' ' + imagefilename[i] + ' ' + strtrim(string(candidate[i]),2) + $
       ;        ' '  + string(correspondence[1:correspondence[0,i],i])
       ; endif else begin
       ;     print, strtrim(string(label[i]),2) + ' ' + imagefilename[i] + ' ' +  strtrim(string(candidate[i]),2)
       ; endelse
       ;endif

    endfor

    readf, lun, line
    stringarray = strsplit(line, ' ', /extract)
    if(strcmp(stringarray[0], 'numgroundtruth:', strlen('numgroundtruth:')) ne 1) then return
    numgroundtruth = fix(stringarray[1])

    for i=0L,numgroundtruth-1 do begin
       readf, lun, line
       stringarray = strsplit(line, ' ', /extract)
       if(i eq 0) then begin
         truthimagefilename = stringarray[0]
         truthnoduleid = fix(stringarray[1])
       endif else begin
         truthimagefilename = [truthimagefilename, stringarray[0]]
         truthnoduleid = [truthnoduleid, fix(stringarray[1])]
       endelse

    endfor

    free_lun, lun
end

pro read_trn, filename, featurelabels, feature, label, candidate, imagefilename, correspondence, $
    truthimagefilename, truthnoduleid, ngroups, groups

    line = strarr(1)

    openr, lun, filename, /get_lun
    if(lun lt 0) then return

    readf, lun, line
    free_lun, lun

    if(strcmp(line[0], '#^TRN V1.0', strlen('#^TRN V1.0')) eq 1) then begin
       read_trn_v10, filename, featurelabels, feature, label, candidate, imagefilename, correspondence, $
         truthimagefilename, truthnoduleid
       ngroups = intarr(n_elements(candidate))
       groups = intarr(10, n_elements(candidate))
       return
    endif

    if(strcmp(line[0], '#^TRN V1.1', strlen('#^TRN V1.1')) eq 1) then begin
       read_trn_v11, filename, featurelabels, feature, label, candidate, imagefilename, correspondence, $
         truthimagefilename, truthnoduleid, ngroups, groups
       return
    endif

end

;------------------------------------------------------------------------------
; Procedure: classify_by_candidate
; Purpose: This procedure classifies candidates by their candidate number.
; A number of classifications are done in which all candidates with a candidate
; nodule ID less than or equal to i are classified as detections. The value of
; i goes from 0 to the maximun number of candidates. This process provides a
; baseline set of classifications for assessing the performance of the initial
; candidate detection algorithm.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;
; Note: This function was changed on 11/11/2003 due to a change made in the
; feature generation system component code (Jay Schildkraut). That code now
; has the option to drop some candidates so the list of candidate indices
; for images may skip some index values. Thus the old approach used in this
; function to limit the number of detections per image will not work properly.
; The modified code actually used the index values to find the n lowest
; candidate indices with n ranging from 1 to the maxnumcandidates.
;------------------------------------------------------------------------------
pro classify_by_candidate, imagefilename, candidate, maxnumcandidates, class=class

    ;  n = (maxnumcandidates < max(candidate)) + 1
    ;  class = intarr(n,n_elements(candidate))
    ;  for i=0L,(n-1) do begin
    ;   class[i,*] = candidate le i
    ;  endfor

    ;--------------------------------------------------------------------------
    ; Go through each image and classify the candidates to take n of the
    ; lowest valued candidates for that image. The value of n will take on
    ; every value in the range 1 to maxnumcanidates (or the number of
    ; candidates for that image if there are fewer than maxnumcandidates
    ; far an image.
    ;--------------------------------------------------------------------------
    uniquenames = imagefilename[uniq(imagefilename)]

    ; Each column of this matrix will eventually contain the classification of
    ; the candidates at a single operating point.
    class = intarr(maxnumcandidates, n_elements(candidate))

    ; Go through each image.
    for i=0L,n_elements(uniquenames)-1 do begin
       indx = where(imagefilename eq uniquenames[i], count)
       if(count gt 0) then begin
         candidateindices = candidate[indx]
         sortedcandidateindices = indx[sort(candidateindices)]
         for n=0L,maxnumcandidates-1 do begin
          actual_n = count < (n+1)
          class[n,sortedcandidateindices[0:actual_n-1]] = 1
         endfor
       endif
    endfor

end

;------------------------------------------------------------------------------
; Procedure: cullcandidates
; Purpose: This procedure applies a culling algorithm to reduce the probability
; of detection of candidates that are in the same group. The though is that
; a group of candidates are those candidates from an image that have segmented
; regions that substantially overlap one another. The groups are formed as
; part of the nodule segmentation algorithm that is external to this IDL
; code. Groups are simply attributes of the candidates as far as this IDL
; code is concerned. The imagefilename, candidate, ngroups, groups, key1 and
; key2 parameters are vectors (of the same length). that indicate various
; attributes of a list of candidates. These candidates can come from different
; images. The attribute key1 indicates the probability of abnormality or the
; suspiciousness of an abnormality when proabilities are not known. The key1order
; parameter indicates whether lower values of the key are more suspicious
; (key1order=2) or that higher values of key1 are more suspicious (key1order=1)
; as probabilities would be. Key2 and the key2order parameters are optional
; if a second level suspiciousness should be employed to resolve the magnitude
; of the suspiciousness when the key1 values are identical for multiple
; candidates. The culledkey is the processed (modified) key1 suspiciousness
; vector that is output after culling is performed.
; Name: Michael Heath, Eastman Kodak Company
; Date: 02/05/2004
;------------------------------------------------------------------------------
pro cullcandidates, imagefilename, candidate, ngroups, groups, key1, key1order, $
    key1cullvalue, cullmethod, culledkey, key2=key2, key2order=key2order

    if((n_elements(key2) ne 0) or (n_elements(key2order) ne 0)) then begin
       print, 'Second key utilization is not yet availiable in cullcandidates. No culling is applied.'
       culledkey = key1
       return
    endif

    ;--------------------------------------------------------------------------
    ; Culling with method 1 removes candidates that are in the same group
    ; as another candidate that has a higher key1 value.
    ;--------------------------------------------------------------------------
    if(cullmethod eq 1) then begin

       uniquenames = imagefilename[uniq(imagefilename)]

       culledkey = key1

       ; Go through each image.
       for i=0L, n_elements(uniquenames)-1 do begin

;print, 'Culling ' + uniquenames[i]

         ; Create list of indices for candidates in this image.
         indices = where(imagefilename eq uniquenames[i])

         ; Sort the key1 values for this image.
         thiskey1 = key1[indices]
         sortedkey1indices = sort(thiskey1)
         if(key1order eq 1) then sortedkey1indices = reverse(sortedkey1indices)

         thiskey1 = key1[indices[sortedkey1indices]]

         ; Compute an array of the nodule id's in each group for this image.
         status = get_translated_source_groups(uniquenames[i], imagefilename, $
                 candidate, ngroups, groups, groupsize, groupmembers)

         ; Go through each candidate in the sorted list of candidates and remove
         ; other candidates that are later in the sorted key1 list when they are
         ; in the same group as a candidate earlier in the sorted key1 list
         thiscandidate = candidate[indices[sortedkey1indices]]
         thisngroups = ngroups[indices[sortedkey1indices]]
         thisgroups = groups[*,indices[sortedkey1indices]]
         culled = 0 * thisngroups

         for c=0L, n_elements(indices) - 1 do begin   ; Go through all of the candidates

          candidateid = thiscandidate[c]

          if(culled[c] eq 1) then continue    ; If this was culled don't invoke culling using this candidate.

          ; Go through all the groups to which this candidate belongs.
          for g=0L, thisngroups[c] - 1 do begin

              groupid = thisgroups[g,c]

              ; Go through all of the candidates in this group.
              for cc=0L, groupsize[groupid] - 1 do begin

;print, 'Candidate ' + strtrim(string(candidateid),2) + ' is in ' + strtrim(string(groupid),2) + ' with:'

                 ; Find the index of this candidate in the list of candidates
                 indx = where(thiscandidate eq groupmembers[cc, groupid], count)

;print, indx

                 if(count ne 0) then begin
                   for n=0L, n_elements(indx) - 1 do begin
                    if((indx[n] gt c) and (culled[indx[n]] eq 0)) then begin
                        thiskey1[indx[n]] = key1cullvalue
                        culled[indx[n]] = 1
                    endif
                   endfor
                 endif
              endfor
          endfor
         endfor

         ; Place these key1 values into the culledkey array at the correct locations for this image.
         culledkey[indices[sortedkey1indices]] = thiskey1

         if(0) then print, 'Culled ' + strtrim(string(total(culled)),2) + ' candidates in ' + uniquenames[i]

       endfor

    endif else begin
       culledkey = key1
       return
    endelse

end

;==============================================================================
; Procedure: traintest_gml_noleaveoneout
; Purpose: To apply a GML classifier training on some data and testing on other
; data.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/26/2003
;==============================================================================
pro traintest_gml_noleaveoneout, training_feature, training_truth, featuresubset, $
    testing_imagefilename, testing_feature, testing_candidate, testing_ngroups, testing_groups, $
    n, class=class, cullmethod=cullmethod, maxapriori=maxapriori

    if(n_elements(maxapriori) eq 0) then max_apriori = 1.0 else max_apriori = maxapriori

    print, 'Training GML (noleaveoneout) with: '
    print, string(featuresubset)

    ;--------------------------------------------------------------------------
    ; Compute mean and covariance for both true and false candidates using
    ; the training data. Use anly the subset of features.
    ;--------------------------------------------------------------------------
    training_featuresubset = training_feature[featuresubset, *]

    t_indices = where(training_truth eq 1, mcount) ; 1 indicates a true nodule
    f_indices = where(training_truth eq 0, scount) ; 0 indicates a false nodule

    t_features = training_featuresubset[*,t_indices]
    f_features = training_featuresubset[*,f_indices]

    t_mean = total(t_features,2) / n_elements(t_indices)
    f_mean = total(f_features,2) / n_elements(f_indices)

    t_features_lessmean = t_features - ((dblarr(n_elements(t_indices))+1.0) ## t_mean)
    f_features_lessmean = f_features - ((dblarr(n_elements(f_indices))+1.0) ## f_mean)

    t_covar = (transpose(t_features_lessmean) ## t_features_lessmean) / double(mcount)
    f_covar = (transpose(f_features_lessmean) ## f_features_lessmean) / double(scount)

    t_inv_covar = invert(t_covar, /double)
    f_inv_covar = invert(f_covar, /double)

    if(n_elements(t_covar) gt 1) then begin
       t_determinant = determ(t_covar, /double)
       f_determinant = determ(f_covar, /double)
    endif else begin
       t_determinant = t_covar[0]
       f_determinant = f_covar[0]
    endelse

    ;--------------------------------------------------------------------------
    ; Extract the subset of features for the testing data.
    ;--------------------------------------------------------------------------
    testing_normfeatureset = testing_feature[featuresubset, *]

    ;--------------------------------------------------------------------------
    ; Go through the candidates for all testing images, one after another. For each
    ; image sequentially process all of its candidates. For each candidate,
    ; compute the
    ;--------------------------------------------------------------------------
    uniquenames = testing_imagefilename[uniq(testing_imagefilename)]

    class = intarr(n,n_elements(testing_imagefilename))

    p_t = dblarr(n_elements(testing_imagefilename))
    p_f = dblarr(n_elements(testing_imagefilename))

    ; Go through each testing image.
    for i=0L, n_elements(uniquenames)-1 do begin

       ; Create list of indices for candidates in this image and candidates
       ; in other images.
       thisimagecandidateindices = where(testing_imagefilename eq uniquenames[i])

       ; Create an array of feature vectors and an array of truth labels
       ; for the candidates in this image.
       thisimagecandidates = testing_normfeatureset[*,thisimagecandidateindices]

       ; Go through the list of candidates in this image.
       for j=0L,n_elements(thisimagecandidateindices)-1 do begin

         ; Compute the distance to each and every candidates feature
         ; vector from the feature vector for this candidate.
         thiscandidatemean = thisimagecandidates[*,j]

         diff_t = thiscandidatemean - t_mean
         diff_f = thiscandidatemean - f_mean

         p_t[thisimagecandidateindices[j]] = exp((-0.5) * diff_t ## (t_inv_covar ## transpose(diff_t))) / $
          (sqrt(t_determinant) * (2.0 * 3.1415926535))

         p_f[thisimagecandidateindices[j]] = exp((-0.5) * diff_f ## (f_inv_covar ## transpose(diff_f))) / $
          (sqrt(f_determinant) * (2.0 * 3.1415926535))

       endfor
    endfor

;   for i=0,n-1 do begin
;     a = (double(i) * max_apriori / double(n-1))
;     b = 1.0 - a
;     class[i,*] = (p_t*a) gt (p_f*b)
;   endfor

    ;--------------------------------------------------------------------------
    ; Set the culling method.
    ;--------------------------------------------------------------------------
    ; cullmethod = 1

    ;--------------------------------------------------------------------------
    ; Classify without culling.
    ;--------------------------------------------------------------------------
    if(cullmethod eq 0) then begin
       for i=0,n-1 do begin
         a = (double(i) * (max_apriori) / double(n-1))
         b = 1.0 - a
         class[i,*] = (p_t*a) gt (p_f*b)
       endfor
       probability = p_t / p_f
    endif

    ;==========================================================================
    ; If the apriori ratios were requested, allocate memory for them.
    ;==========================================================================
    if(n_elements(apriori_ratio) ne 0) then apriori_ratio = dblarr(n)

    ;--------------------------------------------------------------------------
    ; Cull in the classification using the groups information to suppress
    ; candidates in a group with an already classified (as true) candidate
    ; in the group. Suppression completely removes the other candidates in the
    ; group in this method (cullmethod eq 1).
    ;--------------------------------------------------------------------------
    if(cullmethod eq 1) then begin

       print, 'Culling with method 1...'

       key1 = p_t / p_f
       key2 = testing_feature[0,*]

       key1order = 1
       cullcandidates, testing_imagefilename, testing_candidate, testing_ngroups, $
         testing_groups, key1, key1order, 0, cullmethod, culledkey

       ; Go through each operating point.
       for i=0,n-1 do begin

         if(i ne (n-1)) then begin
          a = (double(i) * max_apriori / double(n-1))
          b = 1.0 - a
         endif else begin
          a = (double(i-1) * max_apriori / double(n-1))   ; Just repeat the last operating point because the result can not be computed at the last operating point
          b = 1.0 - a
         endelse

         ;==========================================================================
         ; If the apriori ratios were requested, store them.
         ;==========================================================================
         if(n_elements(apriori_ratio) ne 0) then begin
          if(i ne 0) then apriori_ratio[i] = b / a else apriori_ratio[i] = 0.0
         endif

         if(i eq 0) then begin
          class[i,*] = 0
         endif else begin
          class[i,*] = culledkey gt (b / a)
         endelse

       endfor

       probability = culledkey

    endif

end

;==============================================================================
; Procedure: independent_GML_test
; Purpose: This procedure trains a GML classifier from one set of data and
; evaluates the classifie on a second set of data.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/29/2004
;==============================================================================
pro independent_GML_test, fn_train=fn_train, fn_test=fn_test, featuresubset, $
    output_filename=output_filename, area_featurelabel=area_featurelabel, $
    cullmethod=cullmethod

    numapriori = 300    ; The number of apriori values to use to sweep out the FROC curve

    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    ;==========================================================================
    ;==========================================================================
    ; Prepare the training data!!!!!
    ;==========================================================================
    ;==========================================================================

    read_trn, fn_train, featurelabels_train, feature_train, label_train, $
        candidate_train, imagefilename_train, correspondence_train, $
        truthimagefilename_train, truthnoduleid_train, ngroups_train, groups_train

    ;==========================================================================
    ; Optionally remove the candidates with zero area.
    ;==========================================================================
    if(n_elements(area_featurelabel) ne 0) then begin

       orig_featurelabels = featurelabels_train
       orig_feature = feature_train
       orig_label = label_train
       orig_candidate = candidate_train
       orig_imagefilename = imagefilename_train
       orig_correspondence = correspondence_train
       orig_ngroups = ngroups_train
       orig_groups = groups_train

       featurelabels_train = 0
       feature_train = 0
       label_train = 0
       candidate_train = 0
       imagefilename_train = 0
       correspondence_train = 0
       ngroups_train = 0
       groups_train = 0

       remove_zero_area_candidates, orig_featurelabels, orig_feature, orig_label, $
         orig_candidate, orig_imagefilename, orig_correspondence, orig_ngroups, $
         orig_groups, area_featurelabel, featurelabels_train, feature_train, $
         label_train, candidate_train, imagefilename_train, correspondence_train, $
         ngroups_train, groups_train, zero_candidate_indices_train

    end

    truth_train = intarr(n_elements(imagefilename_train))
    truth_train = truth_train * 0
    val = byte('P')
    indx = where(label_train eq val[0], count)
    print, count
    truth_train[indx] = 1
    classification_train = truth_train * 0


    ;--------------------------------------------------------------------------
    ; Create a combined list of image filenames from the candidate and truth
    ; lists.
    ;--------------------------------------------------------------------------
    combined_imagefilename_train = [imagefilename_train, truthimagefilename_train]
    sorted_combined_imagefilename_train = combined_imagefilename_train[sort(combined_imagefilename_train)]
    uniquenames_train = sorted_combined_imagefilename_train[uniq(sorted_combined_imagefilename_train)]


    ;==========================================================================
    ;==========================================================================
    ; Prepare the testing data!!!!!
    ;==========================================================================
    ;==========================================================================

    read_trn, fn_test, featurelabels_test, feature_test, label_test, $
        candidate_test, imagefilename_test, correspondence_test, $
        truthimagefilename_test, truthnoduleid_test, ngroups_test, groups_test

    ;==========================================================================
    ; Optionally remove the candidates with zero area.
    ;==========================================================================
    if(n_elements(area_featurelabel) ne 0) then begin

       orig_featurelabels = featurelabels_test
       orig_feature = feature_test
       orig_label = label_test
       orig_candidate = candidate_test
       orig_imagefilename = imagefilename_test
       orig_correspondence = correspondence_test
       orig_ngroups = ngroups_test
       orig_groups = groups_test

       featurelabels_test = 0
       feature_test = 0
       label_test = 0
       candidate_test = 0
       imagefilename_test = 0
       correspondence_test = 0
       ngroups_test = 0
       groups_test = 0

       remove_zero_area_candidates, orig_featurelabels, orig_feature, orig_label, $
         orig_candidate, orig_imagefilename, orig_correspondence, orig_ngroups, $
         orig_groups, area_featurelabel, featurelabels_test, feature_test, $
         label_test, candidate_test, imagefilename_test, correspondence_test, $
         ngroups_test, groups_test, zero_candidate_indices_test

    end

    truth_test = intarr(n_elements(imagefilename_test))
    truth_test = truth_test * 0
    val = byte('P')
    indx = where(label_test eq val[0], count)
    print, count
    truth_test[indx] = 1
    classification_test = truth_test * 0


    device, decompose=0
    window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Create a combined list of image filenames from the candidate and truth
    ; lists.
    ;--------------------------------------------------------------------------
    combined_imagefilename_test = [imagefilename_test, truthimagefilename_test]
    sorted_combined_imagefilename_test = combined_imagefilename_test[sort(combined_imagefilename_test)]
    uniquenames_test = sorted_combined_imagefilename_test[uniq(sorted_combined_imagefilename_test)]


    ;--------------------------------------------------------------------------
    ; Create some arrays to hold the FROC curves that are generated by
    ; splitting the data in various ways and training and testing a classifier
    ; with those datasets.
    ;--------------------------------------------------------------------------

    training_imagefilenames = uniquenames_train
    testing_imagefilenames = uniquenames_test

    training_feature = feature_train
    training_truth = truth_train

    testing_imagefilename = imagefilename_test
    testing_feature = feature_test

    testing_candidate = candidate_test

    testing_ngroups = ngroups_test
    testing_groups = groups_test

    retraintest = 1
    maxapriori = 1.0

    while(retraintest eq 1) do begin

       traintest_gml_noleaveoneout, training_feature, training_truth, featuresubset, $
         testing_imagefilename, testing_feature, testing_candidate, testing_ngroups, $
         testing_groups, numapriori, class=class, cullmethod=cullmethod, maxapriori=maxapriori

       ; Extract the list of true nodules (hit) correspondences for only the
       ; testing candidates.
       test_correspondence = correspondence_test

       testing_truthimagefilename = truthimagefilename_test
       testing_truthnoduleid = truthnoduleid_test

       score_froc, imagefilename_test, class, testing_truthimagefilename, testing_truthnoduleid, test_correspondence, tpf, fpi

       ; Determine the index of the operating points that have a higher false
       ; positive per image rate than the false positive rate that is at the
       ; upper limit of integration of the false positive rate that is being
       ; used for the figure of merit calculation
       fpi_indx = where(fpi gt 11.0, fpi_count)

       if(fpi_count eq 0) then break

       ; If less than 1/2 of the numapriori points at which the FROC curve was evaluated
       ; have a false positive rate (not greater than) the false positive rate associated
       ; with the upper limit of integration, update the maximum apriori value to use
       ; to get more false positive samples below that limit of integration.

       if(fpi_indx[0] lt (numapriori/2)) then begin
         maxapriori = maxapriori * (double(fpi_indx[0]+1) / double(numapriori-1))
         print, 'Updating the maxapriori to a value of ' + strtrim(string(maxapriori),2) + '.'
       endif else begin
         retraintest = 0
       endelse

    endwhile

    intarray_to_string, featuresubset, thestring

    title = 'Indepentent GML Evaluation'
    plot_simple_froc, tpf, fpi, overplot=0, title=title, 11.0

    simple_az, tpf, fpi, 5.0, az, lowfpt=3.0
    print, 'Area_under_FROC from 3.0 to 5.0 fpi = ' + strtrim(string(az),2)

    loadct, 0, /silent
    base_fn = strmid(fn_train, strpos(fn_train, '\',/reverse_search)+1, 100)
    xyouts, 10, 5*0.03, 'Train       = ' + base_fn, alignment=1.0, color=0, /data
    base_fn = strmid(fn_test, strpos(fn_test, '\',/reverse_search)+1, 100)
    xyouts, 10, 4*0.03, 'Test        = ' + base_fn, alignment=1.0, color=0, /data
    xyouts, 10, 3*0.03, 'AUFROC(3-5) = ' + strtrim(string(az),2), alignment=1.0, color=0, /data
    if(n_elements(area_featurelabel) ne 0) then begin
        xyouts, 10, 2*0.03, 'RMZA', alignment=1.0, color=0, /data
    endif

    ;--------------------------------------------------------------------------
    ; If an output filename was specified, write the results to a file.
    ;--------------------------------------------------------------------------
    if(n_elements(output_filename) ne 0) then begin

        openw, lunout, output_filename, /get_lun

       printf, lunout, 'Independent GML Test'
       printf, lunout, 'Training_File: ' + fn_train
       printf, lunout, 'Testing_File: ' + fn_test
       array_to_spacedstring, featuresubset, thestring
       printf, lunout, 'FeatureSubset: ' + thestring
       printf, lunout, 'FPI'
       for i=0L,numapriori-1 do begin
         printf, lunout, fpi[i]
       endfor
       printf, lunout
       printf, lunout, 'TPF'
       for i=0L,numapriori-1 do begin
         printf, lunout, tpf[i]
       endfor

       free_lun, lunout
    endif

end

pro run_independent_GML_test

    ;thefn_train = 'C:\l465657\ChestCAD\November222004_AllCR_EDEP_AutoMask.trn'
    thefn_test = 'C:\ChestCAD\Data\2004\AllCR_11_23_2004\AllCR_11_23_2004.trn'

    thefn_train = 'C:\ChestCAD\Data\2004\jsrt_11_22_2004\jsrt_11_22_2004.trn'
    ;thefn_test = 'C:\l465657\ChestCAD\November222004_JSRT_EDEP_AutoMask.trn'

    ; These are the features selected by the GML classifier for the AllCR dataset.
    ;thefeaturesubset = [6,18,0,2,9,13,26,19,22,1,8,20,27]

    ; These are the features selected by the GML classifier for the JSRT dataset.
    thefeaturesubset = [6,0,3,1,20,13,7,25,19,12,18,27,11,4,9,30]

    thecullmethod = 1
    thearea_featurelabel = 'Shape.areaFraction'
    independent_GML_test, fn_train=thefn_train, fn_test=thefn_test, thefeaturesubset, $
        area_featurelabel=thearea_featurelabel, $
        cullmethod=thecullmethod
end


;==============================================================================
; Procedure: estimate_GML_froc_variance
; Purpose: This procedure estimates the variability of the performance by
; randomly splitting the traing/testing dataset into many separate training and
; testing datasets. A GML classifier is then trained and tested on each set.
; The set of test results (FROC curves) can then be used to estimate the
; variability in performance. This variability in performance reflects many
; factors including (amoung others):
; (1) Variability in the ease/difficulty of cases.
; (2) Degree of possible overtraining on the training dataset which may
; result in poor generalization of performance for the test dataset.
; (3) Random assignment of cases to the training or testing set that had no
; candidate nodules due to failure of the lung segmentation algorithm. Please
; note that a fixed randome number generation seed is used to make the results
; repeatable.
; Name: Michael Heath, Eastman Kodak Company
; Date: 11/26/2003
;==============================================================================
pro estimate_GML_froc_variance, fn=fn, featuresubset, numsplits, output_filename=output_filename, $
    area_featurelabel=area_featurelabel, cullmethod=cullmethod

    ;==========================================================================
    ; Use a fixed seed for the random number generator to get repeatable results.
    ;==========================================================================
    ; Init seed for a repeatable sequence:
    seed = 1001L

    numapriori = 300    ; The number of apriori values to use to sweep out the FROC curve

    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    ;base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    ;==========================================================================
    ; Optionally remove the candidates with zero area.
    ;==========================================================================
    if(n_elements(area_featurelabel) ne 0) then begin

       orig_featurelabels = featurelabels
       orig_feature = feature
       orig_label = label
       orig_candidate = candidate
       orig_imagefilename = imagefilename
       orig_correspondence = correspondence
       orig_ngroups = ngroups
       orig_groups = groups

       featurelabels = 0
       feature = 0
       label = 0
       candidate = 0
       imagefilename = 0
       correspondence = 0
       ngroups = 0
       groups = 0

       remove_zero_area_candidates, orig_featurelabels, orig_feature, orig_label, $
         orig_candidate, orig_imagefilename, orig_correspondence, orig_ngroups, orig_groups, area_featurelabel, $
         featurelabels, feature, label, $
         candidate, imagefilename, correspondence, ngroups, groups, zero_candidate_indices

    end

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    device, decompose=0
    window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Create a combined list of image filenames from the candidate and truth
    ; lists.
    ;--------------------------------------------------------------------------
    combined_imagefilename = [imagefilename, truthimagefilename]
    sorted_combined_imagefilename = combined_imagefilename[sort(combined_imagefilename)]
    uniquenames = sorted_combined_imagefilename[uniq(sorted_combined_imagefilename)]

    ;--------------------------------------------------------------------------
    ; Create some arrays to hold the FROC curves that are generated by
    ; splitting the data in various ways and training and testing a classifier
    ; with those datasets.
    ;--------------------------------------------------------------------------
    fpi_array = dblarr(numsplits, numapriori)
    tpf_array = dblarr(numsplits, numapriori)

    for splitnumber=0L,numsplits-1 do begin

       ; Split the cases into two sets randomly. Note that there may not be any
       ; candidates for some of these images.
       randomnumbers = randomu(seed, n_elements(uniquenames))
       sorted_randomnumberindices = sort(randomnumbers)
       num_training = n_elements(uniquenames) / 2
       training_imagefilenames = uniquenames[sorted_randomnumberindices[0:(num_training-1)]]
       testing_imagefilenames = uniquenames[sorted_randomnumberindices[num_training:(n_elements(uniquenames)-1)]]

       split_key = intarr(n_elements(imagefilename))
       for j=0L,n_elements(training_imagefilenames)-1 do begin
         indx = where(imagefilename eq training_imagefilenames[j], cnt)
         if(cnt ne 0) then split_key[indx] = 1
       endfor

       ; These are indices into the list of candidates for the training and testing candidates.
       training_indices = where(split_key eq 1)
       testing_indices = where(split_key eq 0)

       training_feature = feature[*,training_indices]
       training_truth = truth[training_indices]

       testing_imagefilename = imagefilename[testing_indices]
       testing_feature = feature[*,testing_indices]

       ;training_candidate = candidate[training_indices]
       testing_candidate = candidate[testing_indices]

       testing_ngroups = ngroups[testing_indices]
       testing_groups = groups[*, testing_indices]

       retraintest = 1
       maxapriori = 1.0

       while(retraintest eq 1) do begin

         traintest_gml_noleaveoneout, training_feature, training_truth, featuresubset, $
          testing_imagefilename, testing_feature, testing_candidate, testing_ngroups, $
          testing_groups, numapriori, class=class, cullmethod=cullmethod, maxapriori=maxapriori

         ; Extract the list of true nodules (hit) correspondences for only the
         ; testing candidates.
         test_correspondence = correspondence[*, testing_indices]

         ; Extract the truth nodule-id's for the images in the testing set.
         split_key = intarr(n_elements(truthimagefilename))
         for j=0L,n_elements(testing_imagefilenames)-1 do begin
          indx = where(truthimagefilename eq testing_imagefilenames[j], cnt)
          if(cnt ne 0) then split_key[indx] = 1
         endfor
         indx = where(split_key eq 1)
         testing_truthimagefilename = truthimagefilename[indx]
         testing_truthnoduleid = truthnoduleid[indx]

         score_froc, testing_imagefilename, class, testing_truthimagefilename, testing_truthnoduleid, test_correspondence, tpf, fpi

         ; Determine the index of the operating points that have a higher false
         ; positive per image rate than the false positive rate that is at the
         ; upper limit of integration of the false positive rate that is being
         ; used for the figure of merit calculation
         fpi_indx = where(fpi gt 11.0, fpi_count)

         if(fpi_count eq 0) then break

         ; If less than 1/2 of the numapriori points at which the FROC curve was evaluated
         ; have a false positive rate (not greater than) the false positive rate associated
         ; with the upper limit of integration, update the maximum apriori value to use
         ; to get more false positive samples below that limit of integration.

         if(fpi_indx[0] lt (numapriori/2)) then begin
          maxapriori = maxapriori * (double(fpi_indx[0]+1) / double(numapriori-1))
          print, 'Updating the maxapriori to a value of ' + strtrim(string(maxapriori),2) + '.'
         endif else begin
          retraintest = 0
         endelse

       endwhile

;     traintest_gml_noleaveoneout, training_feature, training_truth, featuresubset, $
;      testing_imagefilename, testing_feature, testing_candidate, testing_ngroups, $
;      testing_groups, numapriori, class=class, cullmethod=cullmethod
;
;     ; Extract the list of true nodules (hit) correspondences for only the
;     ; testing candidates.
;     test_correspondence = correspondence[*, testing_indices]
;
;     ; Extract the truth nodule-id's for the images in the testing set.
;     split_key = intarr(n_elements(truthimagefilename))
;     for j=0L,n_elements(testing_imagefilenames)-1 do begin
;      indx = where(truthimagefilename eq testing_imagefilenames[j], cnt)
;      if(cnt ne 0) then split_key[indx] = 1
;     endfor
;     indx = where(split_key eq 1)
;     testing_truthimagefilename = truthimagefilename[indx]
;     testing_truthnoduleid = truthnoduleid[indx]
;
;     score_froc, testing_imagefilename, class, testing_truthimagefilename, testing_truthnoduleid, test_correspondence, tpf, fpi

       fpi_array[splitnumber, *] = fpi
       tpf_array[splitnumber, *] = tpf

       intarray_to_string, featuresubset, thestring

       if(splitnumber eq 0) then begin
         plot_simple_froc, tpf, fpi, overplot=0, title=title, 11.0
       endif else begin
         plot_simple_froc, tpf, fpi, overplot=1, color = colors[splitnumber mod 10], 11.0
       endelse

    endfor

    ;--------------------------------------------------------------------------
    ; If an output filename was specified, write the results to a file.
    ;--------------------------------------------------------------------------
    if(n_elements(output_filename) ne 0) then begin
       openw, lunout, output_filename, /get_lun

       printf, lunout, 'FROC Variance Results'
       printf, lunout, 'Input_Training_File: ' + fn
       array_to_spacedstring, featuresubset, thestring
       printf, lunout, 'FeatureSubset: ' + thestring
       printf, lunout, 'Numsplits: ' + strtrim(string(numsplits),2)
       printf, lunout
       printf, lunout, 'FPI'
       for i=0L,numapriori-1 do begin
         array_to_spacedstring, fpi_array[*,i], thestring
         printf, lunout, thestring
       endfor
       printf, lunout
       printf, lunout, 'TPF'
       for i=0L,numapriori-1 do begin
         array_to_spacedstring, tpf_array[*,i], thestring
         printf, lunout, thestring
       endfor

       free_lun, lunout
    endif

end

pro example_FROCvariance
    area_featurelabel = 'Shape.areaFraction'
    fn = 'C:\ChestCAD\Data\2004\all\all.trn'
    featuresubset = [6,18,0,15,1,7,21,10,20,19,29,27,9,22];All
    numsplits=10
    output_filename = 'C:\ChestCAD\Data\2004\all\all_frocvardata.txt'
    cullmethod = 1
    estimate_GML_froc_variance, fn=fn, featuresubset, numsplits, output_filename=output_filename, $
        area_featurelabel=area_featurelabel, cullmethod=cullmethod

end

;------------------------------------------------------------------------------
; Procedure: traintest_knn
; Purpose: This procedure performs a leave-an-image-out training of a
; classifier using a KNN based classifier. The inputs are
; (1) imagefilename - a one dimensional array of image filenames, one for
;     each candidate nodule in the entire training/testing dataset. Many
;     candidates may come from the same file so many elements of the
;     imagefilename array may be the same.
; (2) feature - a k x n array of n, k element feature vectors, one for each
;     of the n candidates.
; (3) truth - a one dimensional array of n elements (one for each candidate)
;     that indicates whether or not this candidate corresponds with at least
;     one truth region - a one indicates a correspondence and a zero indicates
;     no correspondence
; (4) featuresubset - an array of integers indicating the features to use in
;     training/testing the algorithm - integers are zero based
; (5) n - an array of entries listing the maximum number of neighbors to consider
;     at each operating point to sweep out the FROC curve - the classifier works
;     by classifying a candidate as a nodule if at least num of i neighbors
;     correspond to true nodules - i assumes values from zero to num
; (6) num - the number of neighbors that must correspond to a nodule in order to
;     classify the candidate as a nodule
; (7) class - a matrix of num x (number of candidates) elements that lists the
;     classification of each candidate nodule at each operating point - this
;     is what the procedure produces
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;------------------------------------------------------------------------------
pro traintest_knn, imagefilename, feature, truth, featuresubset, n, num, $
       candidate=candidate, ngroups=ngroups, groups=groups, class=class, $
       detectionlimit=detectionlimit, cullmethod=cullmethod

    print, 'Training KNN with: '
    print, string(featuresubset)

    ;--------------------------------------------------------------------------
    ; Extract the subset of features we want and normalize each feature 0->1.
    ;--------------------------------------------------------------------------
    normfeatureset = feature[featuresubset, *]
    help, normfeatureset
    for i=0,n_elements(featuresubset)-1 do begin
       minval = min(normfeatureset[i,*], max=maxval)
       temp = (normfeatureset[i,*] - minval) / (maxval - minval)
       normfeatureset[i,*] = temp
    endfor

    ;--------------------------------------------------------------------------
    ; Go through the candidates for all images, one after another. For each
    ; image sequentially process all of its candidates. For each candidate,
    ; comute the Euclidean distance to all of the candidates in all of the
    ; other images. Each candidate this produces a list of distances to all of
    ; the other candidates. In a straight forward knn, the labels associated
    ; with the k nearest neighbors would be identified and used to determine
    ; the class for this candidate. Here we look for at least num of the
    ; k nearest neighbors to be a true nodule to classify the candiate
    ; as a true nodule. K assumes values from 0 to n.
    ;--------------------------------------------------------------------------
    uniquenames = imagefilename[uniq(imagefilename)]

    ; Each column of this matrix will eventually contain the classification of
    ; the candidates at a single operating point.
    class = intarr(n_elements(n),n_elements(truth))

    ; Go through each image.
    for i=0L, n_elements(uniquenames)-1 do begin

       ; print, uniquenames[i]

       ; Create list of indices for candidates in this image and candidates
       ; in other images.
       thisimagecandidateindices = where(imagefilename eq uniquenames[i])
       otherimagecandidateindices = where(imagefilename ne uniquenames[i])

       ; Create an array of feature vectors for the candidates in this image.
       thisimagecandidates = normfeatureset[*,thisimagecandidateindices]

       ; Create an array of feature vectors and an array of truth labels
       ; for the candidates in other images.
       otherimagecandidates = normfeatureset[*,otherimagecandidateindices]
       othercandidatelabels = truth[otherimagecandidateindices]
       t_indices = where(othercandidatelabels eq 1, mcount) ; 1 indicates a true nodule
       f_indices = where(othercandidatelabels eq 0, scount) ; 0 indicates a false nodule

       ; Go through the list of candidates in this image. For each candidate we
       ; compute results for all of the operating points.
       for j=0L,n_elements(thisimagecandidateindices)-1 do begin

         ; Compute the distance to each and every candidates feature
         ; vector from the feature vector for this candidate.
         thiscandidatemean = thisimagecandidates[*,j]
         diffvector = otherimagecandidates - $
          ((dblarr(n_elements(otherimagecandidateindices))+1) ## thiscandidatemean)
         distance = sqrt(total(diffvector * diffvector,1))

         distance_sorted_indices = sort(distance)
         closestlabels = othercandidatelabels[distance_sorted_indices]

         cumulativesum = total(closestlabels, /cumulative)

         ; Go through the operating points. Again, these operating points are
         ; the number of neighbors to consider. The array n contains the number of
         ; neighbors to consider for each operating point. The number of entries
         ; in the array n indicates the number of operating points.
         for k=0,n_elements(n)-1 do begin
          if(cumulativesum[n[k]] ge num) then begin
              class[k,thisimagecandidateindices[j]] = cumulativesum[n[k]] ; 1
          endif else begin
              class[k,thisimagecandidateindices[j]] = 0
          endelse
         endfor

       endfor
    endfor

    ;--------------------------------------------------------------------------
    ; Set the culling method.
    ;--------------------------------------------------------------------------
    ; cullmethod = 1

    ;--------------------------------------------------------------------------
    ; Classify without culling.
    ;--------------------------------------------------------------------------
    if(cullmethod eq 0) then begin

       print, 'Not performing culling...'
       tempclass = class < 1
       class = tempclass

    endif

    ;--------------------------------------------------------------------------
    ; Cull in the classification using the groups information to suppress
    ; candidates in a group with an already classified (as true) candidate
    ; in the group. Suppression completely removes the other candidates in the
    ; group in this method (cullmethod eq 1).
    ;--------------------------------------------------------------------------
    if(cullmethod eq 1) then begin

       print, 'Culling with method 1...'

       ; Find the operating point where the candidate passes the clssification.
       ; For this classifier, this is the lowest number of neighbors that needs
       ; to be considered to find at least "num" nodules.
       key1 = intarr(n_elements(candidate))
       for k=0L,n_elements(candidate)-1 do begin
         indx = where(class[*,k] ne 0, count)
         if(count ne 0) then begin
          key1[k] = indx[0]
         endif else begin
          key1[k] = n_elements(n) + 1   ; note that n + 1 will never pass the classifier
         endelse
       endfor

       key1order = 2
       cullcandidates, imagefilename, candidate, ngroups, groups, key1, key1order, (n_elements(n)+1), cullmethod, culledkey

       for k=0,n_elements(candidate)-1 do begin
         if(culledkey[k] ge (n_elements(n)+1)) then class[*,k] = 0
       endfor

       tempclass = class < 1
       class = tempclass

    endif

;     ;----------------------------------------------------------------------
;     ; If there is no detection limit, then every non-zero value in class
;     ; for this image indicates a detection. If however, there is a
;     ; detection limit, then only the "detectionlimit" number of candidates
;     ; that have the smallest class values for this image should be declared
;     ; as detections.
;     ;----------------------------------------------------------------------
;     if(n_elements(detectionlimit) eq 0) then begin
;      tempclass = class[*,thisimagecandidateindices] < 1
;      class[*,thisimagecandidateindices] = tempclass
;     endif else begin
;      for k=0L,n_elements(n)-1 do begin
;
;          ; Extract a copy of the class labels for this operating point for this image. Non zero
;          ; values indicate the number of neighbors it took to declare this candidate as a nodule.
;          tempclass = class[k,thisimagecandidateindices]
;
;          ; Get the indices of the non-zero values.
;          tempclassnonzeroindex = where(tempclass ne 0, count)
;
;          ; Unless we have at least one nonzero value, there is nothing to do.
;          if(count gt 0) then begin
;
;             ; Compute indices into the array of nonzero indices that sort those
;             ; non-zero values in increasing order.
;             sortedtempclassnonzeroindex = sort(tempclass[tempclassnonzeroindex])
;
;             ; Find the number of detections to declare. It is the minimum of the number
;             ; of non-zero indices and the maximum number of detections for any single image.
;             numtouse = count < detectionlimit
;
;             ; Create a temporary array of class labels that is initialized to zero.
;             tempclass2 = intarr(n_elements(thisimagecandidateindices))
;
;             ; Use the indices of the first "numtouse" to reference the nonzero indices and
;             ; use those referenced indices into the array of candidates for this image to mark
;             ; only those candidates as detections.
;             tempclass2[tempclassnonzeroindex[sortedtempclassnonzeroindex[0:(numtouse-1)]]] = 1
;             class[k,thisimagecandidateindices] = tempclass2
;
;          endif
;      endfor
;     endelse

end

;------------------------------------------------------------------------------
; Procedure: traintest_gml
; Purpose: This procedure performs a leave-an-image-out training of a
; classifier using a GML based classifier. The inputs are
; (1) imagefilename - a one dimensional array of image filenames, one for
;     each candidate nodule in the entire training/testing dataset. Many
;     candidates may come from the same file so many elements of the
;     imagefilename array may be the same.
; (2) feature - a k x n array of n, k element feature vectors, one for each
;     of the n candidates.
; (3) truth - a one dimensional array of n elements (one for each candidate)
;     that indicates whether or not this candidate corresponds with at least
;     one truth region - a one indicates a correspondence and a zero indicates
;     no correspondence
; (4) featuresubset - an array of integers indicating the features to use in
;     training/testing the algorithm - integers are zero based
; (5) n - the number of apriori probabilities to use to sweep out the FROC curve
; (6) class - a matrix of num x (number of candidates) elements that lists the
;     classification of each candidate nodule at each operating point - this
;     is what the procedure produces
; (7) detectionlimit - an optional paramter to limit the number of detections
;     declared for each and every image
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;------------------------------------------------------------------------------
pro traintest_gml, imagefilename, feature, truth, featuresubset, n, $
    class=class, detectionlimit=detectionlimit, candidate=candidate, $
          ngroups=ngroups, groups=groups, cullmethod=cullmethod, $
          probability=probability, apriori_ratio=apriori_ratio, maxapriori=maxapriori

    if(n_elements(maxapriori) eq 0) then max_apriori = 1.0 else max_apriori = maxapriori

    print, 'Training GML with: '
    print, string(featuresubset)

    ;--------------------------------------------------------------------------
    ; Extract the subset of features we want.
    ;--------------------------------------------------------------------------
    normfeatureset = feature[featuresubset, *]
    help, normfeatureset

    ;--------------------------------------------------------------------------
    ; Go through the candidates for all images, one after another. For each
    ; image sequentially process all of its candidates. For each candidate,
    ; compute the
    ;--------------------------------------------------------------------------
    uniquenames = imagefilename[uniq(imagefilename)]

    class = intarr(n,n_elements(truth))
    probability = dblarr(n_elements(truth))

    p_t = dblarr(n_elements(imagefilename))
    p_f = dblarr(n_elements(imagefilename))

    ; Go through each image.
    for i=0L, n_elements(uniquenames)-1 do begin

       ; Create list of indices for candidates in this image and candidates
       ; in other images.
       thisimagecandidateindices = where(imagefilename eq uniquenames[i])
       otherimagecandidateindices = where(imagefilename ne uniquenames[i])

       ; Create an array of feature vectors and an array of truth labels
       ; for the candidates in this image.
       thisimagecandidates = normfeatureset[*,thisimagecandidateindices]

       ; Create an array of feature vectors and an array of truth labels
       ; for the candidates in other images.
       otherimagecandidates = normfeatureset[*,otherimagecandidateindices]
       othercandidatelabels = truth[otherimagecandidateindices]

       t_indices = where(othercandidatelabels eq 1, mcount) ; 1 indicates a true nodule
       f_indices = where(othercandidatelabels eq 0, scount) ; 0 indicates a false nodule

       t_features = otherimagecandidates[*,t_indices]
       f_features = otherimagecandidates[*,f_indices]

       t_mean = total(t_features,2) / n_elements(t_indices)
       f_mean = total(f_features,2) / n_elements(f_indices)

       t_features_lessmean = t_features - ((dblarr(n_elements(t_indices))+1.0) ## t_mean)
       f_features_lessmean = f_features - ((dblarr(n_elements(f_indices))+1.0) ## f_mean)

       t_covar = (transpose(t_features_lessmean) ## t_features_lessmean) / double(mcount)
       f_covar = (transpose(f_features_lessmean) ## f_features_lessmean) / double(scount)

       t_inv_covar = invert(t_covar, t_invert_status, /double)
       ;help, t_invert_status
       f_inv_covar = invert(f_covar, f_invert_status, /double)
       ;help, f_invert_status

       ;print, 'TINVSTATUS: ' + strtrim(string(t_invert_status),2) + '     FINVSTATUS: ' +  strtrim(string(f_invert_status),2)

       if(n_elements(t_covar) gt 1) then begin
         t_determinant = determ(t_covar, /double)
         f_determinant = determ(f_covar, /double)
       endif else begin
         t_determinant = t_covar[0]
         f_determinant = f_covar[0]
       endelse

       ; Go through the list of candidates in this image.
       for j=0L,n_elements(thisimagecandidateindices)-1 do begin

         ; Compute the distance to each and every candidates feature
         ; vector from the feature vector for this candidate.
         thiscandidatemean = thisimagecandidates[*,j]

         diff_t = thiscandidatemean - t_mean
         diff_f = thiscandidatemean - f_mean

         p_t[thisimagecandidateindices[j]] = exp((-0.5) * diff_t ## (t_inv_covar ## transpose(diff_t))) / $
          (sqrt(t_determinant) * (2.0 * 3.1415926535))

         p_f[thisimagecandidateindices[j]] = exp((-0.5) * diff_f ## (f_inv_covar ## transpose(diff_f))) / $
          (sqrt(f_determinant) * (2.0 * 3.1415926535))

       endfor

    endfor

    ;--------------------------------------------------------------------------
    ; Set the culling method.
    ;--------------------------------------------------------------------------
    cullmethod = 1

    ;--------------------------------------------------------------------------
    ; Classify without culling.
    ;--------------------------------------------------------------------------
    if(cullmethod eq 0) then begin
       for i=0,n-1 do begin
         a = (double(i) * (max_apriori) / double(n-1))
         b = 1.0 - a
         class[i,*] = (p_t*a) gt (p_f*b)
       endfor
       probability = p_t / p_f
    endif

    ;==========================================================================
    ; If the apriori ratios were requested, allocate memory for them.
    ;==========================================================================
    if(n_elements(apriori_ratio) ne 0) then apriori_ratio = dblarr(n)

    ;--------------------------------------------------------------------------
    ; Cull in the classification using the groups information to suppress
    ; candidates in a group with an already classified (as true) candidate
    ; in the group. Suppression completely removes the other candidates in the
    ; group in this method (cullmethod eq 1).
    ;--------------------------------------------------------------------------
    if(cullmethod eq 1) then begin

       print, 'Culling with method 1...'

       key1 = p_t / p_f
       key2 = feature[0,*]

       key1order = 1
       cullcandidates, imagefilename, candidate, ngroups, groups, key1, key1order, 0, cullmethod, culledkey

       ; Go through each operating point.
       for i=0,n-1 do begin

         if(i ne (n-1)) then begin
          a = (double(i) * max_apriori / double(n-1))
          b = 1.0 - a
         endif else begin
          a = (double(i-1) * max_apriori / double(n-1))   ; Just repeat the last operating point because the result cannot be computed at the last operating point
          b = 1.0 - a
         endelse

         ;==========================================================================
         ; If the apriori ratios were requested, store them.
         ;==========================================================================
         if(n_elements(apriori_ratio) ne 0) then begin
          if(i ne 0) then apriori_ratio[i] = b / a else apriori_ratio[i] = 0.0
         endif

         if(i eq 0) then begin
          class[i,*] = 0
         endif else begin
          class[i,*] = culledkey gt (b / a)
         endelse

       endfor

       probability = culledkey

    endif

;   ;--------------------------------------------------------------------------
;   ; The probabilities are used to classify the candidates. If there is a
;   ; detection limit, then the number of detections are limited for each image.
;   ;--------------------------------------------------------------------------
;   if(n_elements(detectionlimit) eq 0) then begin
;     for i=0,n-1 do begin
;      a = (double(i) / double(n-1))
;      b = 1.0 - a
;      class[i,*] = (p_t*a) gt (p_f*b)
;     endfor
;   endif
;    else begin
;     print, 'Using a detection limit of ' + strtrim(string(detectionlimit),2)
;     for i=0,n-1 do begin
;
;      if(i ne (n-1)) then begin
;          a = (double(i) / double(n-1))
;          b = 1.0 - a
;      endif else begin
;          a = (double(i-1) / double(n-1))    ; Just repeat the last operating point because the result can not be computed at the last operating point
;          b = 1.0 - a
;      endelse
;
;      tempclass = (p_t*a) gt (p_f*b)
;
;      for j=0L, n_elements(uniquenames)-1 do begin
;
;          ; print, uniquenames[i]
;          ; Create list of indices for candidates in this image.
;          thisimagecandidateindices = where((imagefilename eq uniquenames[j]) and (tempclass eq 1), count)
;
;          if(count gt 0) then begin
;
;             ;--------------------------------------------------------------
;             ; If the number of detections for this image exceeds the
;             ; detection limit then the less likely detections are removed
;             ; from the classified detections to limit the actual number
;             ; of detections to the detection limit.
;             ;--------------------------------------------------------------
;             if(count gt detectionlimit) then begin
;               theratio = (p_t[thisimagecandidateindices] * a) / (p_f[thisimagecandidateindices] * b) ; Does not make sense for a=1.0 and b=0.0
;               indx = reverse(sort(theratio))
;               numtokeep = count < detectionlimit
;               candidates_to_keep_index = thisimagecandidateindices[indx[0:(numtokeep-1)]]
;               class[i,candidates_to_keep_index] = 1
;               ;print, 'Limited ' + uniquenames[j] + ' from ' + strtrim(string(count),2) + ' to ' + strtrim(string(n_elements(candidates_to_keep_index)),2)
;             endif else begin
;               class[i,thisimagecandidateindices] = 1
;             endelse
;          endif
;      endfor
;     endfor
;   endelse

end

;------------------------------------------------------------------------------
; Procedure: score_froc
; Purpose: To compute an array of true positive fractions and a corresponding
; array expressing the average number of false positives per image. This data
; is the free-response-operating-characteristic analysis used to express the
; performance of the chest CAD algorithm. The inputs are:
; (1) imagefilename - an array of image filenames, one per candidate nodule
; (2)
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;------------------------------------------------------------------------------
pro score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

    combinednames = [imagefilename,truthimagefilename]
    sortedcombinednames = combinednames[sort(combinednames)]
    uniquenames = sortedcombinednames[uniq(sortedcombinednames)]
    numimages = n_elements(uniquenames)

    tpf = dblarr(n_elements(class[*,0]))
    fpi = dblarr(n_elements(class[*,0]))

    ; Set up a matrix that lists the nodule id's for every image. Images that
    ; have no (true) nodules, but appear in the list of detections will
    ; have entries in this matrix.
    truthmatrix = intarr(max(truthnoduleid)+1, numimages)
    for i=0L,n_elements(truthimagefilename)-1 do begin
       index = where(uniquenames eq truthimagefilename[i], count)
       if(count ne 1) then begin
         print, 'didnot find this once ' + truthimagefilename[i]
         return
       endif
       truthmatrix[truthnoduleid[i],index] = 1
    endfor

    ; Cycle through each operating point.
    for i=0L,n_elements(class[*,0])-1 do begin

       detectedmatrix = 0 * truthmatrix
       count_tp = 0L
       count_fp = 0L
       count_fn = 0L

       detectionindex = where(class[i,*] eq 1, numdetections)

       ; Score the detections.
       for j=0L,numdetections-1 do begin

         index = where(uniquenames eq imagefilename[detectionindex[j]], count)

         nummatch = 0
         numtpgenerated = 0

         for k=1,correspondence[0,detectionindex[j]] do begin
          noduleid = correspondence[k,detectionindex[j]]
          if(truthmatrix[noduleid,index] eq 1) then begin
              nummatch = nummatch + 1
              if(detectedmatrix[noduleid, index] eq 0) then begin
                 numtpgenerated = numtpgenerated + 1
                 detectedmatrix[noduleid, index] = 1
              endif
          endif
         endfor

         if(nummatch eq 0) then count_fp = count_fp + 1

       endfor

       count_tp = total(detectedmatrix)
       count_fn = total(truthmatrix) - count_tp

       tpf[i] = double(count_tp) / double(count_tp + count_fn)
       fpi[i] = double(count_fp) / double(numimages)

    endfor
end

;==============================================================================
; Procedure: simple_az
; Purpose: This procedure computed the area under the FROC curve between zero
; and fpt false positives per image.
; Note: The procedure was updated to integrate from a user specified lower
; false positive rate to the fpt.
; Date: 11/26/2003
;==============================================================================
; Name: Michael Heath
; Date: 10/10/2003
;==============================================================================
pro simple_az, input_tpf, input_fpi, fpt, az, lowfpt=lowfpt

    ;==========================================================================
    ; Add a zero point if one is not already in the data.
    ;==========================================================================
    if(input_fpi[0] ne 0.0) then begin
       fpi = [0.0, input_fpi]
       tpf = [0.0, input_tpf]
    endif else begin
       fpi = input_fpi
       tpf = input_tpf
    endelse

    if(fpi[n_elements(fpi)-1] lt fpt) then begin
       print, 'Error! The Az cannot be computed because the fpi does not go high enough!'
       az = 0
       return
    endif

    area = 0.0

    for i=0L,n_elements(fpi)-2 do begin

       skip = 0

       ; Outside integration.
       if(fpi[i+1] lt lowfpt) then continue
       if(fpi[i] gt fpt) then continue

       ; Wholly inside integration.
       if((fpi[i] ge lowfpt) and (fpi[i+1] le fpt)) then begin
         llfpi = fpi[i]
         ulfpi = fpi[i+1]
         lltpf = tpf[i]
         ultpf = tpf[i+1]
         skip = 1
         action = 'inside'
       endif

       ; Overlap the low end of integration.
       if((skip eq 0) and (fpi[i] lt lowfpt) and (fpi[i+1] ge lowfpt) and (fpi[i+1] le fpt)) then begin
         llfpi = lowfpt
         if(fpi[i+1] eq fpi[i]) then continue
         lltpf = tpf[i] + ((llfpi - fpi[i]) / (fpi[i+1] - fpi[i])) * (tpf[i+1] - tpf[i])
         ulfpi = fpi[i+1]
         ultpf = tpf[i+1]
         skip = 1
         action = 'overlap_low'
       endif

       ; Overlap the high end of integration.
       if((skip eq 0) and (fpi[i] ge lowfpt) and (fpi[i] le fpt) and (fpi[i+1] gt fpt)) then begin
         llfpi = fpi[i]
         lltpf = tpf[i]
         ulfpi = fpt
         if(fpi[i+1] eq fpi[i]) then continue
         ultpf = tpf[i] + ((fpt - fpi[i]) / (fpi[i+1] - fpi[i])) * (tpf[i+1] - tpf[i])
         skip = 1
         action = 'overlap_high'
       endif

       ; Overlap the high end of integration.
       if((skip eq 0) and (fpi[i] le lowfpt) and (fpi[i+1] gt fpt)) then begin
         llfpi = lowfpt
         if(fpi[i+1] eq fpi[i]) then continue
         lltpf = tpf[i] + ((llfpi - fpi[i]) / (fpi[i+1] - fpi[i])) * (tpf[i+1] - tpf[i])
         ulfpi = fpt
         if(fpi[i+1] eq fpi[i]) then continue
         ultpf = tpf[i] + ((fpt - fpi[i]) / (fpi[i+1] - fpi[i])) * (tpf[i+1] - tpf[i])
         skip = 1
         action = 'span'
       endif

       ; Compute the incremental area and add it in.
       incremental_area = (ulfpi - llfpi) * $
          ((min([lltpf,ultpf]) + 0.5 * (max([lltpf,ultpf]) - min([lltpf,ultpf]))))

       area = area + incremental_area

    endfor

    az = area
end

;------------------------------------------------------------------------------
; Procedure: plot_simple_froc
; Purpose: Given a list of candidates, each with a list of ground truth nodules,
; it matches, along with a list of all true nodules, this function computes
; and plots an FROC curve for a set of classifications of the candidates.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/10/2003
;------------------------------------------------------------------------------
pro plot_simple_froc, tpf, fpi, overplot=overplot, title=title, color=color, maxfpitoplot

    if(n_elements(title) eq 0) then begin
       thetitle = 'Title'
    endif else begin
       thetitle = title
    endelse

    ; Create the plot.
    if(n_elements(overplot) eq 0) then begin
       loadct, 0, /silent
       plot, fpi, tpf, xtitle='Average Number of False Positives / Image', ytitle='TPF', title=title, $
         xrange = [0,maxfpitoplot], yrange = [0.0,1.0], xsytle = 1, ystyle = 1, background=255, color=0
    endif else begin
       if(overplot eq 0) then begin
         ;window, xsize=700, ysize=700, /free, retain=2
         loadct, 0, /silent
         plot, fpi, tpf, xtitle='Average Number of False Positives / Image', ytitle='TPF', title=title, $
          xrange = [0,maxfpitoplot], yrange = [0.0,1.0], xstyle=1, ystyle=1, background=255, color=0
       endif else begin
         loadct, 38, /silent
         oplot, fpi, tpf, color=color ; xtitle='Average Number of False Positives / Image', ytitle='TPF'
         loadct, 0, /silent
       endelse
    endelse

end

pro intarray_to_string, thearray, thestring
    for i=0,n_elements(thearray)-1 do begin
       if(n_elements(newstring) eq 0) then begin
         newstring = strtrim(string(thearray[i]),2)
       endif else begin
         newstring = newstring + ',' + strtrim(string(thearray[i]),2)
       endelse
    endfor
    thestring = newstring
end

pro array_to_spacedstring, thearray, thestring
    for i=0,n_elements(thearray)-1 do begin
       if(n_elements(newstring) eq 0) then begin
         newstring = strtrim(string(thearray[i]),2)
       endif else begin
         newstring = newstring + ' ' + strtrim(string(thearray[i]),2)
       endelse
    endfor
    thestring = newstring
end

;------------------------------------------------------------------------------
; Procedure: limit_number_of_candidates
; Purpose: This function limits the number of candidates that are used for each
; image.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/29/2003
;------------------------------------------------------------------------------
pro limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, n

    indx = where(candidate lt n, count)

    print, count

    temp = feature[*,indx]
    feature = temp

    temp = label[indx]
    label = temp

    temp = candidate[indx]
    candidate = temp

    temp = imagefilename[indx]
    imagefilename = temp

    temp = correspondence[*,indx]
    correspondence = temp

    print, max(candidate)
end

pro compute_singlefeature_separation, fn

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    truth[indx] = 1

    s = size(feature, /dimensions)
    numfeatures = s[0]
    numcandidates = s[1]

    nodule_indices = where(truth eq 1, num_nodules)
    nonnodule_indices = where(truth ne 1, num_nonnodules)

    nodule_features = feature[*,nodule_indices]
    nonnodule_features = feature[*,nonnodule_indices]

    nodule_mean = dblarr(numfeatures)
    nodule_stdev = dblarr(numfeatures)
    nonnodule_mean = dblarr(numfeatures)
    nonnodule_stdev = dblarr(numfeatures)
    separability = dblarr(numfeatures)

    for i=0,numfeatures-1 do begin
       nodule_mean[i] = mean(nodule_features[i,*], /double)
       nodule_stdev[i] = stddev(nodule_features[i,*], /double)

       nonnodule_mean[i] = mean(nonnodule_features[i,*], /double)
       nonnodule_stdev[i] = stddev(nonnodule_features[i,*], /double)

       separability[i] = abs(nodule_mean[i] - nonnodule_mean[i]) / ((nodule_stdev[i] + nonnodule_stdev[i]) / 2.0)

       print, strtrim(string(separability[i]),2)

    endfor

    rankindex = reverse(sort(separability))

    t1 = featurelabels[rankindex]
    t2 = separability[rankindex]

    for i=0,numfeatures-1 do begin
       print, '[' + strtrim(string(rankindex[i]),2) + ']  SEPARABILITY=' + strtrim(string(t2[i]),2) + '     ' + t1[i]
    endfor

    bar_plot, separability, barnames = featurelabels, /rotate, /outline

end

;------------------------------------------------------------------------------
; Procedure: knn_search
; Purpose: Apply feature selection to find the best features to use for
; lung nodule detection. Use a KNN classifier, a greedy search of the features
; and a leave an image out training/testing methodology to maximize the
; area under the FROC curve. Sequentially plot results as each new feature is
; selected.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/14/2003
;------------------------------------------------------------------------------
pro knn_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, $
    fn=fn, maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, detectionlimit=detectionlimit, $
    lowfpt=lowfpt, featurestouse=featurestouse, cullmethod=cullmethod, area_featurelabel=area_featurelabel

    docull = 0
    if(n_elements(cullmethod) ne 0) then docull = cullmethod

    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA_july242003.trn'

    ;fn = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'

    kt = 5          ; This many neighbors must correspond to nodules for a candidate to be labeled as a nodule
    ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
    if(n_elements(fpt) eq 0) then fpt = 10.0
    n = 1 + indgen(300)    ; An array of the number of neighbors to consider to sweep out the FROC curve
    numtoselect = 10   ; The number of features to select

    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    ;==========================================================================
    ; Optionally remove the candidates with zero area.
    ;==========================================================================
    if(n_elements(area_featurelabel) ne 0) then begin

       orig_featurelabels = featurelabels
       orig_feature = feature
       orig_label = label
       orig_candidate = candidate
       orig_imagefilename = imagefilename
       orig_correspondence = correspondence
       orig_ngroups = ngroups
       orig_groups = groups

       featurelabels = 0
       feature = 0
       label = 0
       candidate = 0
       imagefilename = 0
       correspondence = 0
       ngroups = 0
       groups = 0

       remove_zero_area_candidates, orig_featurelabels, orig_feature, orig_label, $
         orig_candidate, orig_imagefilename, orig_correspondence, orig_ngroups, orig_groups, area_featurelabel, $
         featurelabels, feature, label, $
         candidate, imagefilename, correspondence, ngroups, groups, zero_candidate_indices

    end

    ;--------------------------------------------------------------------------
    ; Allow only a subset of features to be used.
    ;--------------------------------------------------------------------------
    if(n_elements(featurestouse) ne 0) then begin
       print, 'Using a subset of features'
       temp = featurelabels[featurestouse]
       featurelabels = temp
       print, transpose(featurelabels)
       temp = feature[featurestouse,*]
       feature = temp
    endif

    numtoselect = n_elements(featurelabels)
    azrecord = dblarr(numtoselect)

    if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
    limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    device, decompose=0
    window, xsize=700, ysize=700, xpos=2000, ypos=0, /free, retain=2  ;xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Determine what the maximum possible FROC curve would be by using the
    ; correspondence of the candidates with the truth as the classifications
    ; of the candidate nodules.
    ;--------------------------------------------------------------------------
    tempclass = truth
    class = reform(tempclass, 1, n_elements(truth))
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
    print, perfect_tpf, perfect_fpi

    unselectedfeatures = indgen(n_elements(featurelabels))

    for q=0L,numtoselect-1 do begin

       best_new_az = 0
       best_new_feature = 0

       for i=0,n_elements(unselectedfeatures)-1 do begin

         trialfeature = unselectedfeatures[i]

         if(n_elements(selectedfeatures) eq 0) then begin
          candidatefeatures = [trialfeature]
         endif else begin
          candidatefeatures = [selectedfeatures, trialfeature]
         endelse

         if(n_elements(detectionlimit) eq 0) then begin
          if(docull eq 0) then begin
              traintest_knn, imagefilename, feature, truth, candidatefeatures, n, kt, $
                 candidate=candidate, class=class, cullmethod=docull
          endif else begin
              traintest_knn, imagefilename, feature, truth, candidatefeatures, n, kt, $
                 candidate=candidate, ngroups=ngroups, groups=groups, class=class, cullmethod=docull
          endelse
         endif else begin
          if(docull eq 0) then begin
              traintest_knn, imagefilename, feature, truth, candidatefeatures, n, kt, $
                 candidate=candidate, class=class, detectionlimit=detectionlimit, cullmethod=docull
          endif else begin
              traintest_knn, imagefilename, feature, truth, candidatefeatures, n, kt, $
                 candidate=candidate, ngroups=ngroups, groups=groups, class=class, detectionlimit=detectionlimit, cullmethod=docull
          endelse

         endelse
         score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

         simple_az, tpf, fpi, fpt, az, lowfpt=lowfpt

         if(q eq 1) then begin
          print, 'Stopping'
         endif

         intarray_to_string, candidatefeatures, thestring
         print, strtrim(string(az),2) + ': (' + thestring + ')'

         if(i eq 0) then begin
          best_new_az = az
          best_new_tpf = tpf
          best_new_fpi = fpi
          best_new_feature = trialfeature
          best_class = class
         endif else begin
          if(az gt best_new_az) then begin
              best_new_az = az
              best_new_tpf = tpf
              best_new_fpi = fpi
              best_new_feature = trialfeature
              best_class = class
          endif
         endelse

         ;for i=0L, n_elements(class[*,0])-1 do begin
         ;    plot_classification, feature, featurelabels, 0, 22, truth, class[i,*], $
         ;       useclassification=1, titlecomponent = 'N' + strtrim(string(n[i]),2)
         ;endfor

         class = class * 0

         if((i mod 5) eq 0) then wshow

       endfor

       ;----------------------------------------------------------------------
       ; Add the best new feature to the set of selected features and remove
       ; it from the set of unselected features.
       ;----------------------------------------------------------------------
       if(n_elements(selectedfeatures) eq 0) then begin
         selectedfeatures = [best_new_feature]
       endif else begin
         selectedfeatures = [selectedfeatures, best_new_feature]
       endelse
       indx = where(unselectedfeatures ne best_new_feature, count)
       if(count ne 0) then begin
         newunselectedfeatures = unselectedfeatures[indx]
         unselectedfeatures = newunselectedfeatures
       endif

       intarray_to_string, selectedfeatures, thestring
       thestring = strtrim(string(selectedfeatures[q]),2)

       title = 'KNN(' + strtrim(string(kt),2) + ',' + strtrim(string(lowfpt),2) + ',' + strtrim(string(fpt),2) + ') '
       labeltext = 'FSelect (' + strtrim(string(best_new_az),2) + ') [' + thestring + '] ' + $
         '  ' + featurelabels[selectedfeatures[q]]

       if(q eq 0) then begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=0, title=title, maxfpitoplot
       endif else begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=1, color = colors[q mod 10], maxfpitoplot
       endelse

       if(q eq 0) then begin
         xyouts, 0.5, 0.97 - 0.024 * q, labeltext, alignment=0.0, color=colors[q mod 10], /data
       endif else begin
         loadct, 38, /silent
         xyouts, 0.5, 0.97 - 0.024 * q, labeltext, alignment=0.0, color=colors[q mod 10], /data
         loadct, 0, /silent
       endelse

       if(q eq 0) then begin
         loadct, 38, /silent
         oplot, [maxfpitoplot-1,100], [perfect_tpf,perfect_tpf], color = colors[0]
         if(n_elements(area_featurelabel) ne 0) then $
          xyouts, maxfpitoplot-1, 0.15, alignment=1.0, 'RM_ZEROA', color=colors[0]
         xyouts, maxfpitoplot-1, 0.12, alignment=1.0, 'CULL=' + strtrim(string(docull),2), color=colors[0]
         xyouts, maxfpitoplot-1, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
         xyouts, maxfpitoplot-1, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
          strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
          color = colors[0]
         xyouts, maxfpitoplot-1, 0.03, alignment=1.0, base_fn, color=colors[0]
         loadct, 0, /silent
       endif

       ;======================================================================
       ; Save the classification results to a file.
       ;======================================================================
;     aim_fpi=5
;     save_classification_results, 'd:\test_classification.out', 'KNN Search', fn, imagefilename, candidate, best_new_tpf, best_new_fpi, aim_fpi, best_class
;     return

       azrecord[q] = best_new_az
    endfor

    ;--------------------------------------------------------------------------
    ; Score the initial candidate detection algorithm by selecting an integer
    ; number of candidates per image, where that integer goes from 0 to 12.
    ;--------------------------------------------------------------------------
    classify_by_candidate, imagefilename, candidate, 12, class=class
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
    simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az, lowfpt=lowfpt
    loadct, 0, /silent
    oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
    labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
    xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=0, /data
    if(n_elements(detectionlimit) ne 0) then begin
       xyouts, maxfpitoplot-1, 0.12, 'MaxD/I = ' + strtrim(string(fix(detectionlimit)),2), alignment=1.0, color=0, /data
    endif

    featurenumberrecord = selectedfeatures
    featurelabelrecord = featurelabels[featurenumberrecord]
end

pro save_classification_results, fnout, title, trainingfilename, imagefilename, candidate, tpf, fpi, aim_fpi, class

    operating_point_index = -1
    for i=0,n_elements(fpi)-2 do begin
       if((fpi[i] le aim_fpi) and (fpi[i+1] gt aim_fpi)) then operating_point_index = i
    endfor

    if(operating_point_index eq -1) then return

    print, operating_point_index

    openw, lunout, fnout, /get_lun
    printf, lunout, '#^DET V1.0'
    printf, lunout, 'operatingFPI: ' + strtrim(string(fpi[operating_point_index]),2)
    printf, lunout, 'operatingTPF: ' + strtrim(string(tpf[operating_point_index]),2)
    printf, lunout, 'operatingAimFPI: ' + strtrim(string(aim_fpi),2)
    printf, lunout, 'trainingFileID: ' + trainingfilename

    operatingpointclass = class[operating_point_index,*]

    print, total(operatingpointclass)

    uniquenames = imagefilename[uniq(imagefilename)]
    printf, lunout, 'numImages: ' + strtrim(string(n_elements(uniquenames)),2)

    ; Go through each image.
    for i=0L,n_elements(uniquenames)-1 do begin
       indx = where((imagefilename eq uniquenames[i]) and (operatingpointclass eq 1), count)
       printf, lunout, 'imageID: ' + uniquenames[i]
       printf, lunout, 'imageNumCandidates: ' + strtrim(string(count),2)
       if(count ne 0) then begin
         candidateindexlist = candidate[indx]
         for j=0,count-1 do begin
          printf, lunout, strtrim(string(candidateindexlist[j]),2)
         endfor
       endif
    endfor
    free_lun, lunout
end

;==============================================================================
; Function: find_operating_point_index
; Purpose: Given an sorted array (increasing) of false positives per image and
; an aim false positives per image value, determine the maximum index of the
; array that is less than or equal to the aim false positive value.
; Name: Michael Heath, Eastman Kodak Company
; Date: 3/24/2004
;==============================================================================
function find_operating_point_index, fpi, aim_fpi

    operating_point_index = -1
    for i=0,n_elements(fpi)-2 do begin
       if((fpi[i] le aim_fpi) and (fpi[i+1] gt aim_fpi)) then operating_point_index = i
    endfor

    return, operating_point_index

end

;------------------------------------------------------------------------------
; Procedure: gml_search
; Purpose: Apply feature selection to find the best features to use for
; lung nodule detection. Use a GML classifier, a greedy search of the features
; and a leave an image out training/testing methodology to maximize the
; area under the FROC curve. Sequentially plot results as each new feature is
; selected.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/14/2003
;------------------------------------------------------------------------------
pro gml_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, fn=fn, $
    maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, detectionlimit=detectionlimit, $
    lowfpt=lowfpt, featurestouse=featurestouse, cullmethod=cullmethod, area_featurelabel=area_featurelabel

    docull = 0
    if(n_elements(cullmethod) ne 0) then docull = cullmethod

    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA_july242003.trn'

    ;fn = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'

    ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
    if(n_elements(fpt) eq 0) then fpt = 10.0

    numapriori = 300    ; The number of apriori values to use to sweep out the FROC curve
    numtoselect = 35; The number of features to select


    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    ;==========================================================================
    ; Optionally remove the candidates with zero area.
    ;==========================================================================
    if(n_elements(area_featurelabel) ne 0) then begin

       orig_featurelabels = featurelabels
       orig_feature = feature
       orig_label = label
       orig_candidate = candidate
       orig_imagefilename = imagefilename
       orig_correspondence = correspondence
       orig_ngroups = ngroups
       orig_groups = groups

       featurelabels = 0
       feature = 0
       label = 0
       candidate = 0
       imagefilename = 0
       correspondence = 0
       ngroups = 0
       groups = 0

       remove_zero_area_candidates, orig_featurelabels, orig_feature, orig_label, $
         orig_candidate, orig_imagefilename, orig_correspondence, orig_ngroups, orig_groups, area_featurelabel, $
         featurelabels, feature, label, $
         candidate, imagefilename, correspondence, ngroups, groups, zero_candidate_indices

    end

    ; if(max(ngroups) gt 0) then docull = 1

    ;--------------------------------------------------------------------------
    ; Allow only a subset of features to be used.
    ;--------------------------------------------------------------------------
    if(n_elements(featurestouse) ne 0) then begin
       print, 'Using a subset of features'
       temp = featurelabels[featurestouse]
       featurelabels = temp
       print, transpose(featurelabels)
       temp = feature[featurestouse,*]
       feature = temp
    endif

    numtoselect = n_elements(featurelabels)
    azrecord = dblarr(numtoselect)

    if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
    limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    device, decompose=0
    window, xsize=700, ysize=700, xpos=1300, ypos=0, /free, retain=2    ; xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Determine what the maximum possible FROC curve would be by using the
    ; correspondence of the candidates with the truth as the classifications
    ; of the candidate nodules.
    ;--------------------------------------------------------------------------
    tempclass = truth
    class = reform(tempclass, 1, n_elements(truth))
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
    print, perfect_tpf, perfect_fpi

    unselectedfeatures = indgen(n_elements(featurelabels))

    for q=0L,numtoselect-1 do begin

       best_new_az = 0
       best_new_feature = 0

       for i=0,n_elements(unselectedfeatures)-1 do begin

         trialfeature = unselectedfeatures[i]

         if(n_elements(selectedfeatures) eq 0) then begin
          candidatefeatures = [trialfeature]
         endif else begin
          candidatefeatures = [selectedfeatures, trialfeature]
         endelse

         retraintest = 1
         maxapriori = 1.0

         while(retraintest eq 1) do begin

          if(n_elements(detectionlimit) eq 0) then begin
              if(docull eq 0) then begin
                 traintest_gml, imagefilename, feature, truth, candidatefeatures, numapriori, class=class, cullmethod=docull, maxapriori=maxapriori
              endif else begin
                 traintest_gml, imagefilename, feature, truth, candidatefeatures, numapriori, class=class, $
                   candidate=candidate, ngroups=ngroups, groups=groups, cullmethod=docull, maxapriori=maxapriori
              endelse
          endif else begin
              if(docull eq 0) then begin
                 traintest_gml, imagefilename, feature, truth, candidatefeatures, numapriori, class=class, $
                   detectionlimit=detectionlimit, cullmethod=docull, maxapriori=maxapriori
              endif else begin
                 traintest_gml, imagefilename, feature, truth, candidatefeatures, numapriori, class=class, $
                   detectionlimit=detectionlimit, candidate=candidate, ngroups=ngroups, groups=groups, cullmethod=docull, maxapriori=maxapriori
              endelse
          endelse
          score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

          ; Determine the index of the operating points that have a higher false
          ; positive per image rate than the false positive rate that is at the
          ; upper limit of integration of the false positive rate that is being
          ; used for the figure of merit calculation
          fpi_indx = where(fpi gt fpt, fpi_count)

          if(fpi_count eq 0) then break

          ; If less than 1/2 of the numapriori points at which the FROC curve was evaluated
          ; have a false positive rate (not greater than) the false positive rate associated
          ; with the upper limit of integration, update the maximum apriori value to use
          ; to get more false positive samples below that limit of integration.

          if(fpi_indx[0] lt (numapriori/2)) then begin
              maxapriori = maxapriori * (double(fpi_indx[0]+1) / double(numapriori-1))
              print, 'Updating the maxapriori to a value of ' + strtrim(string(maxapriori),2) + '.'
          endif else begin
              retraintest = 0
          endelse

         endwhile

         simple_az, tpf, fpi, fpt, az, lowfpt=lowfpt

         if(q eq 1) then begin
          print, 'Stopping'
         endif

         intarray_to_string, candidatefeatures, thestring
         print, strtrim(string(az),2) + ': (' + thestring + ')'

         if(i eq 0) then begin
          best_new_az = az
          best_new_tpf = tpf
          best_new_fpi = fpi
          best_new_feature = trialfeature
          best_class = class
         endif else begin
          if(az gt best_new_az) then begin
              best_new_az = az
              best_new_tpf = tpf
              best_new_fpi = fpi
              best_new_feature = trialfeature
              best_class = class
          endif
         endelse

         ;for i=0L, n_elements(class[*,0])-1 do begin
         ;    plot_classification, feature, featurelabels, 0, 22, truth, class[i,*], $
         ;       useclassification=1, titlecomponent = 'N' + strtrim(string(n[i]),2)
         ;endfor

         class = class * 0

         if((i mod 5) eq 0) then wshow

       endfor

       ;----------------------------------------------------------------------
       ; Add the best new feature to the set of selected features and remove
       ; it from the set of unselected features.
       ;----------------------------------------------------------------------
       if(n_elements(selectedfeatures) eq 0) then begin
         selectedfeatures = [best_new_feature]
       endif else begin
         selectedfeatures = [selectedfeatures, best_new_feature]
       endelse
       indx = where(unselectedfeatures ne best_new_feature, count)
       if(count ne 0) then begin
         newunselectedfeatures = unselectedfeatures[indx]
         unselectedfeatures = newunselectedfeatures
       endif

       intarray_to_string, selectedfeatures, thestring
       thestring = strtrim(string(selectedfeatures[q]),2)

       title = 'GML(' + strtrim(string(numapriori),2) + ',' + strtrim(string(lowfpt),2) + ',' + strtrim(string(fpt),2) + ') '
       labeltext = 'FSelect (' + strtrim(string(best_new_az),2) + ') [' + thestring + '] ' + $
         '  ' + featurelabels[selectedfeatures[q]]

       if(q eq 0) then begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=0, title=title, maxfpitoplot
       endif else begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=1, color = colors[q mod 10], maxfpitoplot
       endelse
       ; plot_simple_froc, worst_tpf, worst_fpi, overplot=1

       if(q eq 0) then begin
         xyouts, 0.5, 0.97 - 0.024 * q, labeltext, alignment=0.0, color=colors[q mod 10], /data
       endif else begin
         loadct, 38, /silent
         xyouts, 0.5, 0.97 - 0.024 * q, labeltext, alignment=0.0, color=colors[q mod 10], /data
         loadct, 0, /silent
       endelse
       ;simple_az, imagefilename, truth, class, 10.0

       if(q eq 0) then begin
         loadct, 38, /silent
         oplot, [maxfpitoplot-1,100], [perfect_tpf,perfect_tpf], color = colors[0]
         if(n_elements(area_featurelabel) ne 0) then $
          xyouts, maxfpitoplot-1, 0.15, alignment=1.0, 'RM_ZEROA', color=colors[0]
         xyouts, maxfpitoplot-1, 0.12, alignment=1.0, 'CULL=' + strtrim(string(docull),2), color=colors[0]
         xyouts, maxfpitoplot-1, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
         xyouts, maxfpitoplot-1, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
          strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
          color = colors[0]
         xyouts, maxfpitoplot-1, 0.03, alignment=1.0, base_fn, color=colors[0]
         loadct, 0, /silent
       endif

       ;======================================================================
       ; Save the classification results to a file.
       ;======================================================================
;     aim_fpi=5
;     save_classification_results, 'd:\test_classification.out', 'GML Search', fn, imagefilename, candidate, best_new_tpf, best_new_fpi, aim_fpi, best_class
;     return

       azrecord[q] = best_new_az
    endfor

    ;--------------------------------------------------------------------------
    ; Score the initial candidate detection algorithm by selecting an integer
    ; number of candidates per image, where that integer goes from 0 to 12.
    ;--------------------------------------------------------------------------
    classify_by_candidate, imagefilename, candidate, 12, class=class
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
    simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az, lowfpt=lowfpt
    loadct, 0, /silent
    oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
    labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
    xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=0, /data
    if(n_elements(detectionlimit) ne 0) then begin
       xyouts, maxfpitoplot-1, 0.12, 'MaxD/I = ' + strtrim(string(fix(detectionlimit)),2), alignment=1.0, color=0, /data
    endif

    featurenumberrecord = selectedfeatures
    featurelabelrecord = featurelabels[featurenumberrecord]

end

;------------------------------------------------------------------------------
; Procedure: compute_roc_curve
; Purpose: Compute the ROC curve and store the curve to an ascii file.
; The array named truth stores the truth (whether this candidate
; corresponds to a truth region or not eg. 1 or 0). The matrix class stores
; the class labels (1=P 0=N) as ones or zeros. The rows of the matrix
; correspond to the candidates and the columns correspond to the operating
; points of the classifier.
; Name: Miachel Heath, Eastman Kodak Company
; Date: 12/5/2003
;------------------------------------------------------------------------------
pro compute_roc_curve, truth, class, training_filename, classifier, $
    featureset, featurelabels, roc_filename

    s = size(class, /dimensions)
    numapriori = s[0]
    numcandidates = s[1]

    sensitivity = dblarr(numapriori)
    specificity = dblarr(numapriori)

    for i=0L, numapriori-1 do begin
       classification = reform(class[i,*])

       tp = where((truth eq 1)   and (classification eq 1), num_tp)
       fn = where((truth eq 1) and (classification eq 0), num_fn)

       fp = where((truth eq 0) and (classification eq 1), num_fp)
       tn = where((truth eq 0) and (classification eq 0), num_tn)

       sensitivity[i] = double(num_tp) / double(num_tp + num_fn)
       specificity[i] = double(num_tn) / double(num_fp + num_tn)

    endfor

    window, /free
    plot, 1.0 - specificity, sensitivity, psym=3, title='ROC'

    ;--------------------------------------------------------------------------
    ; The ROC curve plots sensitivity vs. 1 - specificity.  Both spensitivity
    ; and (1-specificity) are somptimes multiplied by 100.0. Here we will
    ; write (1-specificity) and sensitivity to the file.
    ;--------------------------------------------------------------------------
    openw, lunout, roc_filename, /get_lun
    printf, lunout, 'ROC_Output_File'
    printf, lunout, 'Generated: ' + systime()
    printf, lunout, 'Training_Filename: ' + training_filename
    printf, lunout, 'Classifier: ' + classifier
    printf, lunout, 'Feature_Indices:'
    printf, lunout, transpose(featureset)
    printf, lunout, 'Feature_Labels:'
    printf, lunout, transpose(featurelabels[featureset])
    printf, lunout, '1-Specificity Sensitivity'
    for    i=0L, numapriori-1 do begin
       printf, lunout, strtrim(string(1.0 - specificity[i]),2) + ' ' + strtrim(string(sensitivity[i]),2)
    endfor
    free_lun, lunout
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

;------------------------------------------------------------------------------
; Procedure: gml_or_knn_fixedset
; Purpose: Apply training and testing of a GML or a KNN classifier using a
; leave-an-image-out training and testing methodology using a fixed fet of
; features.
; Name: Michael Heath, Eastman Kodak Company
; Date: 12/01/2003
;------------------------------------------------------------------------------
pro gml_or_knn_fixedset, maxfpitoplot, fn=fn, maxcandidatesperimage=maxcandidatesperimage, $
    fpt=fpt, detectionlimit=detectionlimit, lowfpt=lowfpt, fixedfeatureset=fixedfeatureset, $
    classifier=classifier, output_basefilename=output_basefilename, roc_filename=roc_filename, $
    cullmethod=cullmethod, canlist_filename=canlist_filename, outcan_path=outcan_path, $
    area_featurelabel=area_featurelabel

    ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
    if(n_elements(fpt) eq 0) then fpt = 10.0

    if(not((classifier eq 'KNN') or (classifier eq 'GML'))) then begin
       print, 'BAD CLASSIFIER'
       return
    endif

    if(classifier eq 'GML') then begin
       numapriori = 300    ; The number of apriori values to use to sweep out the FROC curve
    endif else begin
       kt = 5          ; This many neighbors must correspond to nodules for a candidate to be labeled as a nodule
       n = 1 + indgen(300)   ; An array of the number of neighbors to consider to sweep out the FROC curve
    endelse

    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups


    ;==========================================================================
    ; Optionally remove the candidates with zero area.
    ;==========================================================================
    if(n_elements(area_featurelabel) ne 0) then begin

       orig_featurelabels = featurelabels
       orig_feature = feature
       orig_label = label
       orig_candidate = candidate
       orig_imagefilename = imagefilename
       orig_correspondence = correspondence
       orig_ngroups = ngroups
       orig_groups = groups

       featurelabels = 0
       feature = 0
       label = 0
       candidate = 0
       imagefilename = 0
       correspondence = 0
       ngroups = 0
       groups = 0

       remove_zero_area_candidates, orig_featurelabels, orig_feature, orig_label, $
         orig_candidate, orig_imagefilename, orig_correspondence, orig_ngroups, orig_groups, area_featurelabel, $
         featurelabels, feature, label, $
         candidate, imagefilename, correspondence, ngroups, groups, zero_candidate_indices

    end

    if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
    limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    device, decompose=0
    window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Determine what the maximum possible FROC curve would be by using the
    ; correspondence of the candidates with the truth as the classifications
    ; of the candidate nodules.
    ;--------------------------------------------------------------------------
    tempclass = truth
    class = reform(tempclass, 1, n_elements(truth))
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
    print, perfect_tpf, perfect_fpi

    ;--------------------------------------------------------------------------
    ; If no list of features was provided, use all of the features.
    ;--------------------------------------------------------------------------
    if(n_elements(fixedfeatureset) ne 0) then begin
       selectedfeatures = fixedfeatureset
    endif else begin
       selectedfeatures = indgen(n_elements(featurelabels))
    endelse

    if(n_elements(cullmethod) ne 0) then docull = 1 else docull = 0

    if(classifier eq 'GML') then begin

       if((classifier eq 'GML') and (n_elements(canlist_filename) ne 0) and (n_elements(outcan_path) ne 0)) then apriori_ratio = 1

;     if(n_elements(detectionlimit) eq 0) then begin
;      traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, candidate=candidate, $
;          ngroups=ngroups, groups=groups, cullmethod=cullmethod, probability=probability, apriori_ratio=apriori_ratio
;     endif else begin
;      traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, detectionlimit=detectionlimit, $
;          candidate=candidate, ngroups=ngroups, groups=groups, cullmethod=cullmethod, probability=probability, apriori_ratio=apriori_ratio
;     endelse


       retraintest = 1
       maxapriori = 1.0

       while(retraintest eq 1) do begin

         if(n_elements(detectionlimit) eq 0) then begin
          if(docull eq 0) then begin
              traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, $
                 candidate=candidate, cullmethod=cullmethod, $
                 probability=probability, apriori_ratio=apriori_ratio, maxapriori=maxapriori
          endif else begin
              traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, $
                 candidate=candidate, ngroups=ngroups, groups=groups, cullmethod=cullmethod, $
                 probability=probability, apriori_ratio=apriori_ratio, maxapriori=maxapriori
          endelse
         endif else begin
          if(docull eq 0) then begin
              traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, $
                 candidate=candidate, detectionlimit=detectionlimit, cullmethod=cullmethod, $
                 probability=probability, apriori_ratio=apriori_ratio, maxapriori=maxapriori
          endif else begin
              traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, $
                 candidate=candidate, detectionlimit=detectionlimit, ngroups=ngroups, groups=groups, cullmethod=cullmethod, $
                 probability=probability, apriori_ratio=apriori_ratio, maxapriori=maxapriori
          endelse
         endelse
         score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

         ; Determine the index of the operating points that have a higher false
         ; positive per image rate than the false positive rate that is at the
         ; upper limit of integration of the false positive rate that is being
         ; used for the figure of merit calculation
         fpi_indx = where(fpi gt fpt, fpi_count)

         if(fpi_count eq 0) then break

         ; If less than 1/2 of the numapriori points at which the FROC curve was evaluated
         ; have a false positive rate (not greater than) the false positive rate associated
         ; with the upper limit of integration, update the maximum apriori value to use
         ; to get more false positive samples below that limit of integration.

         if(fpi_indx[0] lt (numapriori/2)) then begin
          maxapriori = maxapriori * (double(fpi_indx[0]+1) / double(numapriori-1))
          print, 'Updating the maxapriori to a value of ' + strtrim(string(maxapriori),2) + '.'
         endif else begin
          retraintest = 0
         endelse

       endwhile

    endif

    if(classifier eq 'KNN') then begin
       if(n_elements(detectionlimit) eq 0) then begin
         traintest_knn, imagefilename, feature, truth, selectedfeatures, n, kt, $
          candidate=candidate, ngroups=ngroups, groups=groups, class=class, cullmethod=cullmethod
       endif else begin
         traintest_knn, imagefilename, feature, truth, selectedfeatures, n, kt, $
          candidate=candidate, ngroups=ngroups, groups=groups, class=class, detectionlimit=detectionlimit, cullmethod=cullmethod
       endelse
    endif

    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

    simple_az, tpf, fpi, fpt, az, lowfpt=lowfpt

    if(n_elements(selectedfeatures) eq n_elements(featurelabels)) then begin
       thestring = 'ALL-' + strtrim(string(n_elements(featurelabels)),2)
    endif else begin
       intarray_to_string, selectedfeatures, thestring
    endelse
    print, strtrim(string(az),2) + ': (' + thestring + ')'

    if(classifier eq 'GML') then begin
       title = 'GML(' + strtrim(string(numapriori),2) + ',' + strtrim(string(lowfpt),2) + ',' + strtrim(string(fpt),2) + ') '
       labeltext = 'UsrSelected (' + strtrim(string(az),2) + ') [' + thestring + ']'
    endif

    if(classifier eq 'KNN') then begin
       title = 'KNN(' + strtrim(string(kt),2) + ',' + strtrim(string(lowfpt),2) + ',' + strtrim(string(fpt),2) + ') '
       labeltext = 'UsrSelected (' + strtrim(string(az),2) + ') [' + thestring + ']'
    endif

    plot_simple_froc, tpf, fpi, overplot=0, title=title, maxfpitoplot
    xyouts, 0.5, 0.97 - 0.03 * 0, labeltext, alignment=0.0, color=colors[0], /data

    loadct, 38, /silent
    oplot, [maxfpitoplot-1,100], [perfect_tpf,perfect_tpf], color = colors[0]
    xyouts, maxfpitoplot-1, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
    xyouts, maxfpitoplot-1, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
       strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
       color = colors[0]
    xyouts, maxfpitoplot-1, 0.03, alignment=1.0, base_fn, color=colors[0]
    loadct, 0, /silent

    ;--------------------------------------------------------------------------
    ; If a base output filename was provided, then output the classification
    ; results to a file (so that detections could be overlayed on an image
    ; by another program), and output the FROC curve data to a different file.
    ;--------------------------------------------------------------------------
    aim_fpi=4
    if(n_elements(output_basefilename) ne 0) then begin

       ;----------------------------------------------------------------------
       ; Save the classification results to a file.
       ;----------------------------------------------------------------------
       output_filename = output_basefilename + '_classificationresults.txt'

       save_classification_results, output_filename, classifier + ' Search', fn, imagefilename, candidate, tpf, fpi, aim_fpi, class

       ;----------------------------------------------------------------------
       ; Save the FROC results to a text file that can be imported into
       ; Microsoft EXCEL.
       ;----------------------------------------------------------------------
       output_filename = output_basefilename + '_FROCresults.txt'
       openw, lunout, output_filename, /get_lun
       printf, lunout, 'FROC_Result_File_Generated: ' + systime()
       printf, lunout, 'Training_File: ' + fn
       printf, lunout, 'Features_Used_Index:'
       printf, lunout, transpose(selectedfeatures)
       printf, lunout, 'Features_Used_Labels:'
       printf, lunout, transpose(featurelabels[selectedfeatures])
       printf, lunout, title
       printf, lunout, labeltext
       printf, lunout, 'FPI TPF'
       for w=0L, n_elements(fpi)-1 do begin
         printf, lunout, strtrim(string(fpi[w]),2) + ' ' + strtrim(string(tpf[w]),2)
       endfor

       free_lun, lunout

    endif

    ;--------------------------------------------------------------------------
    ; If the user supplied a roc_filename, then compute the ROC curve for
    ; the classifier and write it out to a file.
    ;--------------------------------------------------------------------------
    if(n_elements(roc_filename) ne 0) then begin
       compute_roc_curve, truth, class, fn, classifier, selectedfeatures, featurelabels, roc_filename
    endif

    ;--------------------------------------------------------------------------
    ; If the user ran the 'GML' classifier and specified the name of a file
    ; that contains the list of CAN file filenames and specified the
    ; output directory for storing can files then update each can file to
    ; store the probability of the candidate being a nodule.
    ;--------------------------------------------------------------------------
    if((classifier eq 'GML') and (n_elements(canlist_filename) ne 0) and (n_elements(outcan_path) ne 0)) then begin

       ;----------------------------------------------------------------------
       ; Read in all of the CAN filenames from the file.
       ;----------------------------------------------------------------------
       openr, canlistlun, canlist_filename, /get_lun
       ts = strarr(1)
       readf, canlistlun, ts
       canfilenames = [ts[0]]
        while(not eof(canlistlun)) do begin
           readf, canlistlun, ts
           canfilenames = [canfilenames, ts[0]]
        endwhile
        free_lun, canlistlun

       ; Read in each CAN file to find the name of the image to which it corresponds
       for i=0L, n_elements(canfilenames) - 1 do begin
         status = read_can_file_v11(canfilenames[i], can_data=can_data)
         if(i eq 0) then begin
          canimagelist = *can_data.xraySourceID
         endif else begin
          canimagelist = [canimagelist, *can_data.xraySourceID]
         endelse
         free_can_data, can_data
       endfor

       ; Read in each CAN file and update the probability to be the probability computed by the classifier
       for i=0L, n_elements(canfilenames) - 1 do begin
         status = read_can_file_v11(canfilenames[i], can_data=can_data)

         thisimage_indices = where(imagefilename eq canimagelist[i], count)
         if(count ne 0) then begin
          thisimage_candidate = candidate[thisimage_indices]
          thisimage_probability = probability[thisimage_indices]
         endif

         ; Replace the existing probability with the new one.
         for j=0L, can_data.numCandidates - 1 do begin
          cid = (*can_data.candidate)[j].index
          idx = where(thisimage_candidate eq cid)
          if(ptr_valid((*can_data.candidate)[j].probability) eq 1) then begin
              ptr_free, (*can_data.candidate)[j].probability
          endif
          if(idx[0] eq -1) then begin     ; The candidate was not found, presumable because it had zero area and was removed.
              sprob = strtrim(string(0L), 2)
          endif else begin                 ; Replace the probability with the newly computed one.
              sprob = strtrim(string(thisimage_probability[idx]), 2)
          endelse
          (*can_data.candidate)[j].probability = ptr_new(sprob)
         endfor

         ; Form the name of the new can file to place it in the new directory.
         bfilename = trimpathandsuffix(canfilenames[i])
         outcanfilename = outcan_path + bfilename + '.can'
         status = write_can_file(outcanfilename, can_data=can_data)

         free_can_data, can_data
       endfor

       operating_point_index = find_operating_point_index(fpi, aim_fpi)
       print, 'The apriori_ratio at the operating point is: ' + strtrim(string(apriori_ratio[operating_point_index]),2)
       print, 'At the operating point we got '  + strtrim(string(tpf[operating_point_index] * n_elements(truthnoduleid)),2) + $
         ' of ' + strtrim(string(n_elements(truthnoduleid)),2) + ' nodules.'
    endif

    ;--------------------------------------------------------------------------
    ; Score the initial candidate detection algorithm by selecting an integer
    ; number of candidates per image, where that integer goes from 0 to 12.
    ;--------------------------------------------------------------------------
    class = class * 0
    classify_by_candidate, imagefilename, candidate, 12, class=class
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
    simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az, lowfpt=lowfpt
    loadct, 0, /silent
    oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
    labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
    xyouts, 0.5, 0.97 - 0.03 * 1, labeltext, alignment=0.0, color=0, /data
    if(n_elements(detectionlimit) ne 0) then begin
       xyouts, maxfpitoplot-1, 0.12, 'MaxD/I = ' + strtrim(string(fix(detectionlimit)),2), alignment=1.0, color=0, /data
    endif

end

pro apply_specific_gml_or_knn_run

    fn_March122004_MLM_SetB = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\March122004_MLM_SetB.trn'
    fn_March122004_MLM_SetB_canlist = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\March122004_MLM_SetB_can.txt'

    fn_March262004_SetA = 'G:\detectionalysis\Debug\2004\March262004\March262004_SetA.trn'

    fn_February272004_MLM_JRST154 = 'G:\detectionalysis\Debug\2004\JSRT\February272004\February272004_JSRT154_MLM.trn'

    fn_March262004_SetB = 'G:\detectionalysis\Debug\2004\March262004\March262004_SetB.trn'

    fn_May212004_SetB = 'G:\detectionalysis\Debug\2004\May212004\SetB\May212004_MLM_SetB.trn'

    fn_June022004_MLM_SetB = 'G:\detectionalysis\Debug\2004\June022004\SetB\June022004_MLM_SetB.trn'

    fn_June022004_MLM_JSRT = 'G:\detectionalysis\Debug\2004\June022004\JSRT\June022004_JSRT154logexp_MLM.trn'

    fn_June112004_JSRT154logexp_MLM = 'G:\detectionalysis\Debug\2004\June112004\JSRT\June112004_JSRT154logexp_MLM.trn'

    maxfpitoplot = 11.0
    fpt = 5.0
    lowfpt = 3.0
    classifier = 'GML'
    cullmethod = 1
    area_featurelabel = 'Shape.areaFraction'
    do_outcan = 0

    ;==========================================================================
    ; Do the March 12, 2004 SetB with manual lung masks.
    ;==========================================================================
    fn = fn_March122004_MLM_SetB
    canlist_filename = fn_March122004_MLM_SetB_canlist
    outcan_path = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\OutCan\'
    fixedfeatureset = [0,32,20,5,21,23,15,11,1,8,36,16,18,17,2,14]

    ;==========================================================================
    ; Do the March 26, 2004 SetA with automatic lung masks.
    ;==========================================================================
    fn = fn_March262004_SetA
    ;canlist_filename = fn_March122004_MLM_SetB_canlist
    ;outcan_path = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\OutCan\'
    fixedfeatureset = [0,20,1,16,17,36,18,11,14,39,2,7]
    output_basefilename = 'g:\basefn'

    ;==========================================================================
    ; Do the February27, 2004 JSRT154 with manual lung masks.
    ;==========================================================================
    fn = fn_February272004_MLM_JRST154
    ;canlist_filename = fn_March122004_MLM_SetB_canlist
    ;outcan_path = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\OutCan\'
    fixedfeatureset = [1,0,22,5,29,25,12,27,33,9,26,10,13,16,20,14,15,7,18]
    output_basefilename = 'g:\February272004_KNN_basefn'
    classifier = 'KNN'

    ;==========================================================================
    ; GML for SetB March26.
    ;==========================================================================
    fn = fn_March262004_SetB
    ;canlist_filename = fn_March122004_MLM_SetB_canlist
    ;outcan_path = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\OutCan\'
    fixedfeatureset = [0,20,1,16,2,5,17,34,15]
    output_basefilename = 'g:\March262004_SetB_GML9feature_basefn'
    classifier = 'GML'

    ;==========================================================================
    ; mKNN for SetB March26.
    ;==========================================================================
    fn = fn_March262004_SetB
    ;canlist_filename = fn_March122004_MLM_SetB_canlist
    ;outcan_path = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\OutCan\'
    fixedfeatureset = [0,20,10,39,1,23,18,6,9,7,17,33,15,34,8]
    output_basefilename = 'g:\March262004_SetB_mKNN15feature_basefn'
    classifier = 'KNN'

    ;==========================================================================
    ; GML for SetB March26.
    ;==========================================================================
    fn = fn_May212004_SetB
    ;canlist_filename = fn_March122004_MLM_SetB_canlist
    ;outcan_path = 'G:\detectionalysis\Debug\2004\TemporaryMarch122004\OutCan\'
    fixedfeatureset = [21,19,23,29,2,35,0]
    output_basefilename = 'g:\May212004_SetB_GML_debug'
    classifier = 'GML'

    ;==========================================================================
    ; GML for SetB June022004 for SetB.
    ;==========================================================================
    fn = fn_June022004_MLM_SetB
    canlist_filename = 'G:\detectionalysis\Debug\2004\June022004\SetB\June022004_MLM_SetB_can.txt'
    outcan_path = 'G:\detectionalysis\Debug\2004\June022004\SetB\OutCan\'
    roc_filename = 'G:\detectionalysis\Debug\2004\June022004\SetB\OutCan\June022004_MLM_SetB_14featureGML_ROC'
    fixedfeatureset = [0,20,21,17,1,12,13,5,41,40,11,16,28,26]
    output_basefilename = 'g:\June022004_SetB_GML_debug'
    classifier = 'GML'

    ;==========================================================================
    ; Do the July 11, 2004 SetA with automatic lung masks.
    ;==========================================================================
;   fn_July112004_AutoMask_SetA = 'G:\detectionalysis\Debug\2004\July112004\SetA\July112004_AutoMask_SetA.trn'
;   ;fn_July112004_AutoMask_SetA_canlist = 'G:\detectionalysis\Debug\2004\July112004\SetA\July112004_AutoMask_SetA_can.txt'
;   fn = fn_July112004_AutoMask_SetA
;   canlist_filename = fn_July112004_AutoMask_SetA_canlist
;   outcan_path = 'G:\detectionalysis\Debug\2004\July112004\SetA\OutCan\'
;   fixedfeatureset = [0,15,5,10,21,13,20,11,6]
;   output_basefilename = 'G:\detectionalysis\Debug\2004\July112004\SetA\OutCan\GML_KNNrunoutput_'

    ;==========================================================================
    ; Do the July 11, 2004 SetB with automatic lung masks.
    ;==========================================================================
;   fn_July112004_AutoMask_SetB = 'G:\detectionalysis\Debug\2004\July112004\SetB\July112004_AutoMask_SetB.trn'
;   ;fn_July112004_AutoMask_SetB_canlist = 'G:\detectionalysis\Debug\2004\July112004\SetB\July112004_AutoMask_SetB_can.txt'
;   fn = fn_July112004_AutoMask_SetB
;   ;canlist_filename = fn_July112004_AutoMask_SetB_canlist
;   outcan_path = 'G:\detectionalysis\Debug\2004\July112004\SetB\OutCan\'
;   ;fixedfeatureset = [0,10,6,5,20,26,15,7]
;   fixedfeatureset = [10,0,5,18,19,21,13,25,27,12,15,20,26,22]
;   output_basefilename = 'G:\detectionalysis\Debug\2004\July112004\SetB\OutCan\mKNN_runoutput_'
;   classifier = 'KNN'

    ;==========================================================================
    ; Do the July 11, 2004 AllCR with automatic lung masks (Features from SetB).
    ;==========================================================================
    fn_July112004_AutoMask_AllCR = 'G:\detectionalysis\Debug\2004\July112004\AllCR\July112004_AutoMask_AllCR.trn'
    ;fn_July112004_AutoMask_SetB_canlist = 'G:\detectionalysis\Debug\2004\July112004\SetB\July112004_AutoMask_SetB_can.txt'
    fn = fn_July112004_AutoMask_AllCR
    ;canlist_filename = fn_July112004_AutoMask_SetB_canlist
    outcan_path = 'G:\detectionalysis\Debug\2004\July112004\SetB\OutCan\'
    ;fixedfeatureset = [0,10,6,5,20,26,15,7]
   ; fixedfeatureset = [10,0,5,18,19,21,13,25,27,12,15,20,26,22]

    output_basefilename = 'G:\detectionalysis\Debug\2004\July112004\AllCR\OutCan\mKNN_runoutput_'
    classifier = 'KNN'

    ;==========================================================================
    ; Do a sample run for Joe.
    ;==========================================================================
    do_outcan = 1
    fn_forjoe = 'C:\ChestCAD\Data\2004\All\All.trn' ; JR-Here
    fn = fn_forjoe

    ;fixedfeatureset = [0,12,10,18,11,21,5,13,20];GML Aug 25 2004
    ;fixedfeatureset = [0,12,6,13,2,18,5,11,20,14];KNN Aug 25 2004

    ;fixedfeatureset = [0, 9,    20,     3,     1,    19,    13,    11,     7,    14,    10,     2,     4,    12];GML SEPT 30 2004
    ;fixedfeatureset = [ 0,     9,     5,    16,    14,    13,    20,    19,    21,    15,    18,    12,     2,     1,     6,     8,     4,    22];KNN SEPT 30 2004

    ;fixedfeatureset =[ 1,    10,    13,    19,    23,    14,    22];OCT 1 2004 KNN
    ;fixedfeatureset = [1,    10,     6,    21,    13,    17,    23,     2,     3,    20,    22,     9,     0,    14,    15,     4];OCT 1 2004 GML

    ;fixedfeatureset =[1,     6,    17,    13,    10,    12,    21,     8,     9,    18]; Oct 5 2004 GML
    ;fixedfeatureset = [1,    10,    13,     5,     7,     6,    14,    22,    21,    11,    16,    17,    20,    18,     0,    12,     8]; Oct 5 2004 KNN

    ;fixedfeatureset = [5,    13,     2,    15,     4,    20,    22,     7,    14,    19,    21]; Oct 6 2004 KNN
     ;fixedfeatureset =[5,    13,    21,    11,    14,     3,     0,    15,    17,     4,     6,    19]; Oct 6 2004 GML

  ;fixedfeatureset = [0,4,2,20,21,22,10,23,18,5,8,17,6,13,3,12];setB_10_13_2004 GML
  ;fixedfeatureset =[0,16,20,22,14,21,12,2,13,15,4];setB_10_13_2004 KNN

    ;fixedfeatureset =[6,21,4,8,16,2,13,22];setB_10_20_2004 KNN
    ;fixedfeatureset =[1,0,8,7,5,13,4,23,18,12];setB_10_20_2004 GML

    ;fixedfeatureset =[0,1,11,9,16,10,25,21,22,27,13,17,18,15,19];setB_10_21_2004 GML

;fixedfeatureset =[0,24,25,5,20,26,2,21,19,18,17,14,15,12,10,13,4,1,16];setB_10_21_2004 KNN

;fixedfeatureset =[1,0,9,21,4,2,13,27,16,20,12,24,26,7];setB_10_22_2004 GML
;fixedfeatureset = [4,1,12,3,21,28,18,0,26,23,22,2,5,16];setB_10_25_2004 GML
;fixedfeatureset = [0,4,17,3,26,8,6,24,18,27,1,29,30,23,19,2,9,15,5,14,7,13];jsrt_10_25_2004 GML
;fixedfeatureset = [4,1,2,24,27,0,20,6,18,16,13,26,15];setb_10_27_2004
;fixedfeatureset = [3,20,8,11,27,30,23,13,10,25,12,7];jsrt_10_27_2004
;fixedfeatureset = [3,12,29,15,28,9,0,7,22,25,5,20,21,8,30,17,13,16,26,19];jsrt_10_27_2004
;fixedfeatureset = [0,41,1,33,38,29,32,36,17,5,20,27,8,7,15,16,40,24,22,11];setb_10_27_2004
;fixedfeatureset = [17,30,31,16,21,22,10,12,8];setB_11_1_2004
;fixedfeatureset = [17,0,4,1,32,11,10,18,7,25,24,8];jsrt_11_1_2004
;fixedfeatureset = [6,0,4,32,26,18,17,27,7,22,20,1,9,25,29,8,28];setB_11_18_2004
;fixedfeatureset = [6,2,9,0,7,10,25,18,21,1,30,15,31,32,8,16,26,29,22,23];jsrt_11_18_2004
;fixedfeatureset =[6,0,3,1,20,13,7,25,19,12,18,27,11,4,9,30];jsrt_11_22_2004
;fixedfeatureset =[6,0,5,26,32,9,2,29,31,8,27,1,20,16,25,18,21,17,4,28];setB_11_22_2004
;fixedfeatureset =[6,18,0,2,9,13,26,19,22,1,8,20,27];AllCR_11_23_2004
fixedfeatureset =[6,18,0,15,1,7,21,10,20,19,29,27,9,22];All
    output_basefilename = 'C:\ChestCAD\Data\2004\All\All' ; JR-Here
    classifier = 'GML'
    if(do_outcan) then begin
        canlist_filename = 'C:\ChestCAD\Data\2004\All\all_can.txt'
        outcan_path =      'C:\ChestCAD\Data\2004\All\OutCan\'
    endif

    ;==========================================================================
    ; GML for SetB June022004 for JSRT.
    ;==========================================================================
;   fn = fn_June022004_MLM_JSRT
;   canlist_filename = 'G:\detectionalysis\Debug\2004\June022004\JSRT\June022004_JSRT154logexp_MLM_can.txt'
;   outcan_path = 'G:\detectionalysis\Debug\2004\June022004\JSRT\OutCan\'
;   roc_filename = 'G:\detectionalysis\Debug\2004\June022004\JSRT\OutCan\June022004_MLM_JSRT154logexp_07featureGML_ROC'
;   fixedfeatureset = [0,5,38,39,1,31,27]
;   output_basefilename = 'g:\June022004_JSRT164logexp_GML_debug'
;   classifier = 'GML'

    ;==========================================================================
    ; GML for SetB June112004 for JSRT.
    ;==========================================================================
;   fn = fn_June112004_JSRT154logexp_MLM
;   canlist_filename = 'G:\detectionalysis\Debug\2004\June112004\JSRT\June112004_JSRT154logexp_MLM_can.txt'
;   outcan_path = 'G:\detectionalysis\Debug\2004\June112004\JSRT\OutCan\'
;   roc_filename = 'G:\detectionalysis\Debug\2004\June112004\JSRT\OutCan\June112004_MLM_JSRT154logexp_16featureGML_ROC'
;   fixedfeatureset = [0,5,4,8,2,41,16,7,1,39,14,31,35,22,43,12]
;   output_basefilename = 'g:\June112004_JSRT154logexp_GML_debug'
;   classifier = 'GML'

    ;==========================================================================
    ; Do the run on the synthetic data provided by Joe Revelli in July 2004.
    ;==========================================================================
;   fn_BiModal_fset_2_50src = 'G:\detectionalysis\Debug\2004\July112004\Synthetic\BiModal_fset_2_50src.trn'
;   ;fn_July112004_AutoMask_SetB_canlist = 'G:\detectionalysis\Debug\2004\July112004\SetB\July112004_AutoMask_SetB_can.txt'
;   fn = fn_BiModal_fset_2_50src
;   ;canlist_filename = fn_July112004_AutoMask_SetB_canlist
;   ;outcan_path = 'G:\detectionalysis\Debug\2004\July112004\SetB\OutCan\'
;   fixedfeatureset = [0,1]
;   output_basefilename = 'G:\detectionalysis\Debug\2004\July112004\Synthetic\BiModal_fset_2_50src_GMLrunoutput_'

    if(do_outcan) then begin
        gml_or_knn_fixedset, maxfpitoplot, fn=fn, maxcandidatesperimage=maxcandidatesperimage, $
            fpt=fpt, lowfpt=lowfpt, fixedfeatureset=fixedfeatureset, $   ; detectionlimit=detectionlimit
         classifier=classifier, output_basefilename=output_basefilename, $ ;, $ ;roc_filename=roc_filename, $
          cullmethod=cullmethod, canlist_filename=canlist_filename, outcan_path=outcan_path, $
           area_featurelabel=area_featurelabel
    endif else begin
        gml_or_knn_fixedset, maxfpitoplot, fn=fn, maxcandidatesperimage=maxcandidatesperimage, $
          fpt=fpt, lowfpt=lowfpt, fixedfeatureset=fixedfeatureset, $   ; detectionlimit=detectionlimit
         classifier=classifier, output_basefilename=output_basefilename, $ ;, $ ;roc_filename=roc_filename, $
            cullmethod=cullmethod, $ ; , canlist_filename=canlist_filename, outcan_path=outcan_path, $ ;, $
            area_featurelabel=area_featurelabel
    endelse

;   gml_or_knn_fixedset, maxfpitoplot, fn=fn, maxcandidatesperimage=maxcandidatesperimage, $
;     fpt=fpt, lowfpt=lowfpt, fixedfeatureset=fixedfeatureset, $   ; detectionlimit=detectionlimit
;     classifier=classifier, output_basefilename=output_basefilename, $  ; roc_filename=roc_filename, $
;     cullmethod=cullmethod, area_featurelabel=area_featurelabel, canlist_filename=canlist_filename, outcan_path=outcan_path, $
;     area_featurelabel=area_featurelabel
end



pro try_different_num_features

    ;--------------------------------------------------------------------------
    ; These training files were used for the October 30, 2003 algorithm
    ; evaluation. The SetA files may have incorrect nodule and candidate
    ; numbers. These incorrect indices did not affect the FROC evaluation,
    ; but could affect indexing of the candidate files to view the locations
    ; of candidates or truth (coordinates).
    ;--------------------------------------------------------------------------
    ;  fnA = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
    ;  fnB = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
    ;  fnC = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA_july242003.trn'
    ;
    ;  fn1 = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
    ;  fn2 = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
    ;  fn3 = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'
    ;
    ;  fn_a = 'G:\detectionalysis\Debug\November262003\November262003_60.trn'
    ;  fn_b = 'G:\detectionalysis\Debug\November262003\November262003_77.trn'
    ;  fn_c = 'G:\detectionalysis\Debug\November262003\November262003_SetA.trn'
    ;  fn_d = 'G:\detectionalysis\Debug\November262003\November262003_137.trn'

    ;--------------------------------------------------------------------------
    ; These training files are used for the December 11, 2003 review. The
    ; files contain features and truth for the current algorithm (November263003)
    ; as well as historical data from the December182003 and July242003 reviews.
    ;--------------------------------------------------------------------------
    fnA = 'g:\detectionalysis\Debug\December112003_Review\December182002\December182002_137.trn'
    fnB = 'g:\detectionalysis\Debug\December112003_Review\December182002\December182002_60.trn'
    fnC = 'g:\detectionalysis\Debug\December112003_Review\December182002\December182002_77.trn'
    fnD = 'g:\detectionalysis\Debug\December112003_Review\December182002\December182002_SetA.trn'
    fn1 = 'g:\detectionalysis\Debug\December112003_Review\July242003\July242003_137.trn'
    fn2 = 'g:\detectionalysis\Debug\December112003_Review\July242003\July242003_60.trn'
    fn3 = 'g:\detectionalysis\Debug\December112003_Review\July242003\July242003_77.trn'
    fn4 = 'g:\detectionalysis\Debug\December112003_Review\July242003\July242003_SetA.trn'
    fn_a = 'g:\detectionalysis\Debug\December112003_Review\November262003\November262003_137.trn'
    fn_b = 'g:\detectionalysis\Debug\December112003_Review\November262003\November262003_60.trn'
    fn_c = 'g:\detectionalysis\Debug\December112003_Review\November262003\November262003_77.trn'
    fn_d = 'g:\detectionalysis\Debug\December112003_Review\November262003\November262003_SetA.trn'

    fn_AA = 'g:\detectionalysis\Debug\December112003_Review\July242003_MLM\July242003_137_MLM.trn'
    fn_AAA = 'g:\detectionalysis\Debug\December112003_Review\July242003\July242003_137.trn'
    fn_AAAA = 'g:\detectionalysis\Debug\December112003_Review\July242003_MLM\July242003_SetA_MLM.trn'

    fn2004_A = 'G:\detectionalysis\Debug\2004\January062004\January062004_137.trn'
    fn2004_B = 'G:\detectionalysis\Debug\2004\January062004\January062004_60.trn'
    fn2004_C = 'G:\detectionalysis\Debug\2004\January062004\January062004_77.trn'
    fn2004_D = 'G:\detectionalysis\Debug\2004\January062004\January062004_SetA.trn'

    ;fnarr = [fnA, fnB, fnC, fn1, fn2, fn3]
    fnarr = [fn2004_D]
    ;fnarr = ['G:\detectionalysis\Debug\November112003\setA_groundtruth\setA_november112003.trn']
    ;fnarr = ['G:\detectionalysis\Debug\November172003\setA_groundtruth\november172003_setA.trn']

    fn_January282004 = 'G:\detectionalysis\Debug\2004\January282004\January282004_SetA.trn'

    fn_January282004_MLM_SetB = 'G:\detectionalysis\Debug\2004\January282004\January282004_MLM_SetB.trn'
    ;fn_February032004_MLM_SetB = 'G:\detectionalysis\Debug\2004\February032004\February032004_MLM_SetB.trn'
    fn_July242003_MLM_SetB = 'G:\detectionalysis\Debug\December112003_Review\July242003_MLM\July242003_MLM_SetB.trn'

    fn_January272004_MLM_SetB = 'G:\detectionalysis\Debug\2004\January272004\January272004_SetB.trn'
    fn_February032004_MLM_SetB = 'G:\detectionalysis\Debug\2004\February032004\February032004_MLM_SetB.trn'
    fn_February122004_MLM_SetB = 'G:\detectionalysis\Debug\2004\February122004\February122004_MLM_SetB.trn'

    fn_February232004_MLM_SetB = 'G:\detectionalysis\Debug\2004\February232004\February232004_MLM_SetB.trn'

    fn_February272004_MLM_JRST247 = 'G:\detectionalysis\Debug\2004\JSRT\February272004\February272004_JSRT247_MLM.trn'
    fn_February272004_MLM_JRST154 = 'G:\detectionalysis\Debug\2004\JSRT\February272004\February272004_JSRT154_MLM.trn'

    fn_March152004_JSRT154_MLM = 'G:\detectionalysis\Debug\2004\JSRT\March152004\March152004_JSRT154_MLM.trn'

    fn_March122004_MLM_SetB = 'G:\detectionalysis\Debug\2004\March122004\March122004_MLM_SetB.trn'

    fn_March262004_137 = 'G:\detectionalysis\Debug\2004\March262004\March262004_137.trn'
    fn_March262004_SetA = 'G:\detectionalysis\Debug\2004\March262004\March262004_SetA.trn'
    fn_March262004_SetB = 'G:\detectionalysis\Debug\2004\March262004\March262004_SetB.trn'

    fn_March262004_60 = 'G:\detectionalysis\Debug\2004\March262004\March262004_60.trn'

    ;==========================================================================
    ; Kodak processing of the rendered images used for the Deus evaluation.
    ;==========================================================================
    fn_March262004_deus_condition1_SetA = 'G:\detectionalysis\Debug\2004\Deus\Condition1\March262004_deus_condition1_SetA.trn'
    fn_March262004_deus_condition2_SetA = 'G:\detectionalysis\Debug\2004\Deus\Condition2\March262004_deus_condition2_SetA.trn'

    fn_May192004_SetB = 'G:\detectionalysis\Debug\2004\May192004\SetB\May192004_MLM_SetB.trn'
    fn_May212004_SetB = 'G:\detectionalysis\Debug\2004\May212004\SetB\May212004_MLM_SetB.trn'


    fn_May192004_JSRT154logexp_MLM = 'G:\detectionalysis\Debug\2004\May192004\JSRT\May192004_JSRT154logexp_MLM.trn'

    fn_June022004_MLM_SetB = 'G:\detectionalysis\Debug\2004\June022004\SetB\June022004_MLM_SetB.trn'

    fn_June022004_JSRT154logexp_MLM = 'G:\detectionalysis\Debug\2004\June022004\JSRT\June022004_JSRT154logexp_MLM.trn'

;    fnarr = [fn_February122004_MLM_SetB, fn_February122004_MLM_SetB, $
;       fn_February032004_MLM_SetB, fn_February032004_MLM_SetB, $
;       fn_January272004_MLM_SetB, fn_January272004_MLM_SetB]


    fn_June112004_MLM_SetB = 'G:\detectionalysis\Debug\2004\June112004\SetB\June112004_MLM_SetB.trn'

    fn_June112004_JSRT154logexp_MLM = 'G:\detectionalysis\Debug\2004\June112004\JSRT\June112004_JSRT154logexp_MLM.trn'

    fn_July092004_AutoMask_SetB = 'G:\detectionalysis\Debug\2004\July092004\July092004_AutoMask_SetB.trn'

    fn_July112004_AutoMask_SetB = 'G:\detectionalysis\Debug\2004\July112004\SetB\July112004_AutoMask_SetB.trn'

    fn_July112004_AutoMask_NYPH60 = 'G:\detectionalysis\Debug\2004\July112004\NYPH60\July112004_AutoMask_NYPH60.trn'

    fn_July112004_AutoMask_SetA = 'G:\detectionalysis\Debug\2004\July112004\SetA\July112004_AutoMask_SetA.trn'

    fn_July112004_AutoMask_AllCR = 'G:\detectionalysis\Debug\2004\July112004\AllCR\July112004_AutoMask_AllCR.trn'

    fn_July112004_AutoMask_JSRT154logexp = 'G:\detectionalysis\Debug\2004\July112004\JSRT\July112004_AutoMask_JSRT154logexp.trn'

    fn_BiModal_fset_2_50src = 'G:\detectionalysis\Debug\2004\July112004\Synthetic\BiModal_fset_2_50src.trn'

    ; JoeRevelli - FeatureSelection code to modify.

    fn_joetest = 'C:\ChestCAD\Data\2004\All\All.trn'     ; JR-Here
    fnarr = [fn_joetest]                                          ; JR-Here

    cullmethods = [1]

    n = [70]
    maxfpitoplot = 11

    fpt=5.0
    lowfpt=3.0

    area_featurelabel = 'Shape.areaFraction'                                ; JR-?Here to control zero area removal

    outfn_gml = ['C:\ChestCAD\Data\2004\All\All_GML.txt']   ; JR-Here
    outfn_knn = ['C:\ChestCAD\Data\2004\All\All_mKNN.txt']  ; JR-Here

    ;--------------------------------------------------------------------------
    ; A list of features can be supplied to limit the features that are
    ; considered.
    ;--------------------------------------------------------------------------
    ; featurestouse = indgen(19)

    ; Set of features used in July 24th review selected on 60 image set.
    ; ProSymMean rdens NormYPos vloc LocalVar11 GMVar11 LocalVar21 GMVar21 CoinNorm
    ; featurestouse = [14, 2, 16, 4, 7, 9, 8, 10, 0, 20]

    for k=0L, n_elements(fnarr)-1 do begin

       ; Do the GML
       for i=0L, n_elements(n)-1 do begin
         maxcandidatesperimage = n[i]
         if(n_elements(featurestouse) eq 0) then begin
          gml_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, $
              fn=fnarr[k], maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, lowfpt=lowfpt, $
              cullmethod=cullmethods[k], area_featurelabel=area_featurelabel
         endif else begin
          gml_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, $
              fn=fnarr[k], maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, lowfpt=lowfpt, $
              featurestouse=featurestouse, cullmethod=cullmethods[k], area_featurelabel=area_featurelabel
         endelse
         if(i eq 0) then begin
          selectedfeature_az = dblarr(n_elements(n),n_elements(azrecord))
          selectedfeature_number = intarr(n_elements(n),n_elements(azrecord))
          selectedfeature_labels = strarr(n_elements(n),n_elements(azrecord))
         endif

         selectedfeature_az[i,*] = azrecord
         selectedfeature_number[i,*] = featurenumberrecord
         selectedfeature_labels[i,*] = featurelabelrecord
       endfor

       openw, lunout, outfn_gml[k], /get_lun
       printf, lunout, 'GML Results:'
       printf, lunout, fnarr[k]
       printf, lunout, n
       printf, lunout, selectedfeature_az
       printf, lunout, selectedfeature_number
       printf, lunout, selectedfeature_labels
       free_lun, lunout

;     if(k eq 0) then gml_selectedfeature_az = selectedfeature_az else gml_selectedfeature_az = [gml_selectedfeature_az, selectedfeature_az]
;     if(k eq 0) then gml_selectedfeature_number = selectedfeature_number else gml_selectedfeature_number = [gml_selectedfeature_number, selectedfeature_number]
;     if(k eq 0) then gml_selectedfeature_labels = selectedfeature_labels else gml_selectedfeature_labels = [gml_selectedfeature_labels, selectedfeature_labels]


       print, 'Here'


       ; Do the KNN
;       for i=0L, n_elements(n)-1 do begin
;         maxcandidatesperimage = n[i]
;         if(n_elements(featurestouse) eq 0) then begin
;          knn_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, $
;              fn=fnarr[k], maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, lowfpt=lowfpt, $
;              cullmethod=cullmethods[k], area_featurelabel=area_featurelabel
;         endif else begin
;          knn_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, $
;              fn=fnarr[k], maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, lowfpt=lowfpt, $
;              featurestouse=featurestouse, cullmethod=cullmethods[k], area_featurelabel=area_featurelabel
;         endelse
;         if(i eq 0) then begin
;          selectedfeature_az = dblarr(n_elements(n),n_elements(azrecord))
;          selectedfeature_number = intarr(n_elements(n),n_elements(azrecord))
;          selectedfeature_labels = strarr(n_elements(n),n_elements(azrecord))
;         endif
;
;         selectedfeature_az[i,*] = azrecord
;         selectedfeature_number[i,*] = featurenumberrecord
;         selectedfeature_labels[i,*] = featurelabelrecord
;       endfor
;
    openw, lunout, outfn_knn[k], /get_lun
       printf, lunout, 'mKNN Results:'
       printf, lunout, fnarr[k]
       printf, lunout, n
       printf, lunout, selectedfeature_az
       printf, lunout, selectedfeature_number
       printf, lunout, selectedfeature_labels
       free_lun, lunout

;     if(k eq 0) then knn_selectedfeature_az = selectedfeature_az else knn_selectedfeature_az = [knn_selectedfeature_az, selectedfeature_az]
;     if(k eq 0) then knn_selectedfeature_number = selectedfeature_number else knn_selectedfeature_number = [knn_selectedfeature_number, selectedfeature_number]
;     if(k eq 0) then knn_selectedfeature_labels = selectedfeature_labels else knn_selectedfeature_labels = [knn_selectedfeature_labels, selectedfeature_labels]
    endfor


    print, 'Nearly finished'
end


function generate_random_subset, seed, features

    n = n_elements(features)

    r = randomu(seed, n)

    i = sort(r)

    p = i[0]

    r = randomu(seed, n)

    i = sort(r)

    s = i[0:p]

    print, p
    print, s

    return, s
end


pro try_number_of_gml

    fn = 'g:\detectionalysis\Debug\December112003_Review\July242003\July242003_60.trn'

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    featurestouse = indgen(19)
    cullmethod = 0
    ;--------------------------------------------------------------------------
    ; Allow only a subset of features to be used.
    ;--------------------------------------------------------------------------
    if(n_elements(featurestouse) ne 0) then begin
       print, 'Using a subset of features'
       temp = featurelabels[featurestouse]
       featurelabels = temp
       print, transpose(featurelabels)
       temp = feature[featurestouse,*]
       feature = temp
    endif

    seed = 1001L
    numapriori = 300
    fpt = 5.0
    lowfpt = 3.0

    max_az = 0.0

    openw, lunout, 'g:\try_gml_july242003_60_manyrandomcombinations.txt', /get_lun
    for p=0L,2000 do begin

       selectedfeatures = generate_random_subset(seed, featurelabels)

       traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, cullmethod=cullmethod

       score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

       simple_az, tpf, fpi, fpt, az, lowfpt=lowfpt

       intarray_to_string, selectedfeatures, thestring

       indx = sort(selectedfeatures)
       sortedselectedfeatures = selectedfeatures[indx]

       intarray_to_string, sortedselectedfeatures, thestring

       printf, lunout, strtrim(string(n_elements(sortedselectedfeatures)),2) + ',' + strtrim(string(az),2) + ',' + thestring

       if(az gt max_az) then begin
         max_az = az
         q = p + 1
         maxline = '**** BEST **** ' + strtrim(string(q),2) + ' [' + strtrim(string(max_az),2) + '] ' + strtrim(string(n_elements(sortedselectedfeatures)),2) + ',' + strtrim(string(az),2) + ',' + thestring
       endif

       print, maxline
    endfor

    free_lun, lunout
end

;------------------------------------------------------------------------------
; Procedure: knn_fixedset
; Purpose: Calculate and plot the FROC curves for a set of features using a
; KNN classifier with a leave-an-image-out training/testing methodology.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/14/2003
;------------------------------------------------------------------------------
;pro knn_fixedset, fn=fn, maxcandidatesperimage=maxcandidatesperimage
;
;   ;fn = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
;   ;selectedfeatures = [31,0,17,29,26,35,10,6,15,25]
;   ;fn = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
;   ;selectedfeatures = [0,20,34,27,7,14,8,25,6,10]
;
;   ;fn = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
;   ;selectedfeatures = [20,0,16,7,9,8,3,11,10,6]
;   ;fn = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
;   ;selectedfeatures = [0,4,3,19,6,18,9,23,7,15]
;   ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'
;   ;selectedfeatures = [1,17,4,0,16,29,8,22,20,19]
;
;   kt = 5              ; This many neighbors must correspond to nodules for a candidate to be labeled as a nodule
;   fpt = 10.0          ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
;   n = 1 + indgen(300) ; An array of the number of neighbors to consider to sweep out the FROC curve
;
;   colors = [0,2,4,6,8,11,12,14,15,16] * 14
;
;   base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)
;
;   selectedsets = intarr(n_elements(selectedfeatures),n_elements(selectedfeatures))
;   numinset = intarr(n_elements(selectedfeatures))
;   for i=0,n_elements(selectedfeatures)-1 do begin
;     selectedsets[*,i] = selectedfeatures
;     numinset[i] = 1+i
;   endfor
;
;   read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
;     truthimagefilename, truthnoduleid, ngroups, groups
;
;   if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
;   limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage
;
;   truth = intarr(n_elements(imagefilename))
;   truth = truth * 0
;   val = byte('P')
;   indx = where(label eq val[0], count)
;   print, count
;   truth[indx] = 1
;   classification = truth * 0
;
;   device, decompose=0
;   window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2
;
;   ;--------------------------------------------------------------------------
;   ; Determine what the maximum possible FROC curve would be by using the
;   ; correspondence of the candidates with the truth as the classifications
;   ; of the candidate nodules.
;   ;--------------------------------------------------------------------------
;   tempclass = truth
;   class = reform(tempclass, 1, n_elements(truth))
;   score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
;   print, perfect_tpf, perfect_fpi
;
;   for q=0,n_elements(numinset)-1 do begin
;
;     selectedfeatures = selectedsets[0:(numinset[q]-1),q]
;
;     traintest_knn, imagefilename, feature, truth, selectedfeatures, n, kt, candidate=candidate, ngroups=ngroups, groups=groups, class=class, cullmethod=cullmethod
;
;     score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
;
;     simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az
;
;     intarray_to_string, selectedfeatures, thestring
;     print, strtrim(string(best_new_az),2) + ': (' + thestring + ')'
;
;     class = class * 0
;
;     intarray_to_string, selectedfeatures, thestring
;
;     title = 'KNN(' + strtrim(string(kt),2) + ',' + strtrim(string(fpt),2) + ') '
;     labeltext = 'Selected (' + strtrim(string(best_new_az),2) + ') [' + thestring + '] ' + $
;      '  ' + featurelabels[selectedfeatures[q]]
;
;     if(q eq 0) then begin
;      plot_simple_froc, best_new_tpf, best_new_fpi, overplot=0, title=title
;     endif else begin
;      plot_simple_froc, best_new_tpf, best_new_fpi, overplot=1, color = colors[q]
;     endelse
;     ; plot_simple_froc, worst_tpf, worst_fpi, overplot=1
;
;     if(q eq 0) then begin
;
;      xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
;     endif else begin
;      loadct, 38, /silent
;      xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
;      loadct, 0, /silent
;     endelse
;     ;simple_az, imagefilename, truth, class, 10.0
;
;     if(q eq 0) then begin
;      loadct, 38, /silent
;      oplot, [10,100], [perfect_tpf,perfect_tpf], color = colors[0]
;      xyouts, 10.0, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
;      xyouts, 10.0, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
;          strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
;          color = colors[0]
;      xyouts, 10.0, 0.03, alignment=1.0, base_fn, color=colors[0]
;      loadct, 0, /silent
;     endif
;
;   endfor
;
;   ;--------------------------------------------------------------------------
;   ; Score the initial candidate detection algorithm by selecting an integer
;   ; number of candidates per image, where that integer goes from 0 to 12.
;   ;--------------------------------------------------------------------------
;   classify_by_candidate, candidate, 12, class=class
;   score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
;   simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az
;   loadct, 0, /silent
;   oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
;   labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
;   xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=0, /data
;end

;------------------------------------------------------------------------------
; Procedure: gml_fixedset
; Purpose: Calculate and plot the FROC curves for a set of features using a
; GML classifier with a leave-an-image-out training/testing methodology.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/14/2003
;------------------------------------------------------------------------------
;pro gml_fixedset, fn=fn, maxcandidatesperimage=maxcandidatesperimage
;
;   ;fn = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
;   ;selectedfeatures = [0,4,20,17,29,5,6,33,12,16]
;   ;fn = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
;   ;selectedfeatures = [0,4,7,17,23,32,22,21,3,1]
;
;   ;fn = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
;   ;selectedfeatures = [0,4,13,16,18,6,26,5,8,15]
;   ;fn = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
;   ;selectedfeatures = [0,4,7,17,16,32,25,8,3,11]
;   ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'
;   ;selectedfeatures = [0,4,7,3,15,17,26,16,9,10]
;
;   fpt = 10.0          ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
;   numapriori = 300    ; The number of apriori values to use to sweep out the FROC curve
;
;   colors = [0,2,4,6,8,11,12,14,15,16] * 14
;
;   base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)
;
;   selectedsets = intarr(n_elements(selectedfeatures),n_elements(selectedfeatures))
;   numinset = intarr(n_elements(selectedfeatures))
;   for i=0,n_elements(selectedfeatures)-1 do begin
;     selectedsets[*,i] = selectedfeatures
;     numinset[i] = 1+i
;   endfor
;
;   read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
;     truthimagefilename, truthnoduleid, ngroups, groups
;
;   if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
;   limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage
;
;   truth = intarr(n_elements(imagefilename))
;   truth = truth * 0
;   val = byte('P')
;   indx = where(label eq val[0], count)
;   print, count
;   truth[indx] = 1
;   classification = truth * 0
;
;   device, decompose=0
;   window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2
;
;   ;--------------------------------------------------------------------------
;   ; Determine what the maximum possible FROC curve would be by using the
;   ; correspondence of the candidates with the truth as the classifications
;   ; of the candidate nodules.
;   ;--------------------------------------------------------------------------
;   tempclass = truth
;   class = reform(tempclass, 1, n_elements(truth))
;   score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
;   print, perfect_tpf, perfect_fpi
;
;   for q=0,n_elements(numinset)-1 do begin
;
;     selectedfeatures = selectedsets[0:(numinset[q]-1),q]
;
;     traintest_gml, imagefilename, feature, truth, selectedfeatures, numapriori, class=class, cullmethod=cullmethod
;     score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
;
;     simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az
;
;     intarray_to_string, selectedfeatures, thestring
;     print, strtrim(string(best_new_az),2) + ': (' + thestring + ')'
;
;     class = class * 0
;
;     intarray_to_string, selectedfeatures, thestring
;
;     title = 'GML(' + strtrim(string(numapriori),2) + ',' + strtrim(string(fpt),2) + ') '
;     labeltext = 'Selected (' + strtrim(string(best_new_az),2) + ') [' + thestring + '] ' + $
;      '  ' + featurelabels[selectedfeatures[q]]
;
;     if(q eq 0) then begin
;      plot_simple_froc, best_new_tpf, best_new_fpi, overplot=0, title=title
;     endif else begin
;      plot_simple_froc, best_new_tpf, best_new_fpi, overplot=1, color = colors[q]
;     endelse
;     ;plot_simple_froc, worst_tpf, worst_fpi, overplot=1
;
;     if(q eq 0) then begin
;      xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
;     endif else begin
;      loadct, 38, /silent
;      xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
;      loadct, 0, /silent
;     endelse
;     ;simple_az, imagefilename, truth, class, 10.0
;
;     if(q eq 0) then begin
;      loadct, 38, /silent
;      oplot, [10,100], [perfect_tpf,perfect_tpf], color = colors[0]
;      xyouts, 10.0, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
;      xyouts, 10.0, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
;          strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
;          color = colors[0]
;      xyouts, 10.0, 0.03, alignment=1.0, base_fn, color=colors[0]
;      loadct, 0, /silent
;     endif
;
;   endfor
;
;   ;--------------------------------------------------------------------------
;   ; Score the initial candidate detection algorithm by selecting an integer
;   ; number of candidates per image, where that integer goes from 0 to 12.
;   ;--------------------------------------------------------------------------
;   classify_by_candidate, candidate, 12, class=class
;   score_froc, imagefilename, tclass, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
;   simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az
;   loadct, 0, /silent
;   oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
;   labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
;   xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=0, /data
;end

;------------------------------------------------------------------------------
; Program: plotchestclassification.pro
; Purpose: This IDL program performs feature selection, classification and
; FROC analysis for the chest CAD project.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/20/2003
;------------------------------------------------------------------------------

pro plot_classification, features, featurenames, xaxisfeaturenumber, yaxisfeaturenumber, truth, classification, $
    useclassification=useclassification, titlecomponent=titlecomponent

    window, xsize=700, ysize=700, /free, retain=2
    thiswindowindex = !d.window

    device, decompose = 0
    loadct, 0, /silent

    xaxisfeatures = reform(features[xaxisfeaturenumber,*])
    yaxisfeatures = reform(features[yaxisfeaturenumber,*])

    maxxfeaturevalue = max(xaxisfeatures, min=minxfeaturevalue)
    maxyfeaturevalue = max(yaxisfeatures, min=minyfeaturevalue)

    if(xaxisfeaturenumber lt 10) then begin
       baseoutputfn_part = 'plot_0' + strtrim(string(xaxisfeaturenumber),2) + '_'
    endif else begin
       baseoutputfn_part = 'plot_' + strtrim(string(xaxisfeaturenumber),2) + '_'
    endelse
    if(yaxisfeaturenumber lt 10) then begin
       baseoutputfn = baseoutputfn_part + '0' + $
         strtrim(string(yaxisfeaturenumber),2)
    endif else begin
       baseoutputfn = baseoutputfn_part + $
       strtrim(string(yaxisfeaturenumber),2)
    endelse

    if(n_elements(titlecomponent) ne 0) then begin
       thetitle = 'SetA: October 07, 2003 : ' + ' (' + titlecomponent + ') ' + baseoutputfn
    endif else begin
       thetitle = 'SetA: October 07, 2003 : ' + baseoutputfn
    endelse

    plot, [minxfeaturevalue], [maxxfeaturevalue], psym = 3, $
       background = 255, $
       color = 0, $
       xrange = [minxfeaturevalue, maxxfeaturevalue], $
       yrange = [minyfeaturevalue, maxyfeaturevalue], $
       xtitle = featurenames[xaxisfeaturenumber], $
       ytitle = featurenames[yaxisfeaturenumber], $
       title = thetitle, $
       xstyle = 1, $
       ystyle = 1

    loadct, 38, /silent

    plotwithclassification = 0
    if(n_elements(useclassification) ne 0) then begin
       if(useclassification ne 0) then plotwithclassification = 1
    endif

    ;==========================================================================
    ; If there are no classification results, just make the plot show the
    ; truth by the color and/or shape of the positive and negative truths.
    ;==========================================================================
    if(plotwithclassification eq 0) then begin

       truth_negative_symbol = 2   ; asterisk
       truth_negative_color = 192
       truth_positive_symbol = 8   ; user defined
       truth_positive_color = 64

       truth_negative_index = where(truth eq 0, count_truth_negative)
       truth_positive_index = where(truth eq 1, count_truth_positive)

       ;==========================================================================
       ; Plot the features using symbols and or colors to differentiate the
       ; instances of features relating to true and false nodules. Plot the
       ; negative instances first so positive instances will be displayed on top
       ; of the negative ones. The reason for doing this is that the number of
       ; negative instances are likely to be greater than the positive ones.
       ;==========================================================================
       if(count_truth_negative ne 0) then begin
         loadct, 0, /silent
         oplot, xaxisfeatures[truth_negative_index], yaxisfeatures[truth_negative_index], $
          psym = truth_negative_symbol, $
          color = truth_negative_color, $
          symsize = 1.0
       endif

       if(count_truth_positive ne 0) then begin
         loadct, 38, /silent
         x = cos(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         y = sin(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         usersym, x, y, /fill      ; , COLOR=value] [, /FILL] [, THICK=value]
         oplot, xaxisfeatures[truth_positive_index], yaxisfeatures[truth_positive_index], $
          psym = truth_positive_symbol, $
          color = truth_positive_color, $
          symsize = 1.0
         loadct, 0, /silent
         usersym, x, y      ; , COLOR=value] [, /FILL] [, THICK=value]
         oplot, xaxisfeatures[truth_positive_index], yaxisfeatures[truth_positive_index], $
          psym = truth_positive_symbol, $
          color = 0, $
          symsize = 1.0
       endif

    endif else begin

       loadct, 38, /silent

       tp_symbol = 8   ; user defined
       tp_color = 64
       tn_symbol = 2   ; asterisk
       tn_color = 192

       fp_symbol = 2   ; asterisk
       fp_color = 255
       fn_symbol = 8   ; user defined
       fn_color = 210

       tp_index = where((truth eq 1) and (classification eq 1), count_tp)
       tn_index = where((truth eq 0) and (classification eq 0), count_tn)
       fp_index = where((truth eq 0) and (classification eq 1), count_fp)
       fn_index = where((truth eq 1) and (classification eq 0), count_fn)

       if(count_tn ne 0) then begin
         loadct, 0, /silent
         oplot, xaxisfeatures[tn_index], yaxisfeatures[tn_index], $
          psym = tn_symbol, $
          color = tn_color, $
          symsize = 1.0
       endif

       if(count_fp ne 0) then begin
         loadct, 38, /silent
         oplot, xaxisfeatures[fp_index], yaxisfeatures[fp_index], $
          psym = fp_symbol, $
          color = fp_color, $
          symsize = 1.0

         ; = cos(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         ; = sin(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         ;sersym, x, y      ; , COLOR=value] [, /FILL] [, THICK=value]
         ;oadct, 0, /silent
         ;plot, xaxisfeatures[fp_index], yaxisfeatures[fp_index], $
         ;psym = 8, $
         ;color = 0, $
         ;symsize = 1.5
       endif



       if(count_tp ne 0) then begin
         x = cos(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         y = sin(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         usersym, x, y, /fill      ; , COLOR=value] [, /FILL] [, THICK=value]
         loadct, 38, /silent
         oplot, xaxisfeatures[tp_index], yaxisfeatures[tp_index], $
          psym = tp_symbol, $
          color = tp_color, $
          symsize = 1.0
         loadct, 0, /silent
         usersym, x, y      ; , COLOR=value] [, /FILL] [, THICK=value]
         oplot, xaxisfeatures[tp_index], yaxisfeatures[tp_index], $
          psym = fn_symbol, $
          color = 0, $
          symsize = 1.0
       endif

       if(count_fn ne 0) then begin
         x = cos(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         y = sin(2.0 * 3.1415926535 * (indgen(30,/double)/29.0))
         usersym, x, y, /fill      ; , COLOR=value] [, /FILL] [, THICK=value]
         loadct, 38, /silent
         oplot, xaxisfeatures[fn_index], yaxisfeatures[fn_index], $
          psym = fn_symbol, $
          color = fn_color, $
          symsize = 1.0
         loadct, 0, /silent
         usersym, x, y      ; , COLOR=value] [, /FILL] [, THICK=value]
         oplot, xaxisfeatures[fn_index], yaxisfeatures[fn_index], $
          psym = fn_symbol, $
          color = 0, $
          symsize = 1.0
       endif
    endelse

    loadct, 0, /silent

    ;thisimage = tvrd(0, 0, 700, 700, /order, true=1)
    ;outputfilename = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\scatterplots\' + baseoutputfn + ' ' + titlecomponent + '.png'
    ;write_png, outputfilename, thisimage, /order
    ;wait, 1.0
    ;wdelete, thiswindowindex
end

;------------------------------------------------------------------------------
; Procedure: custom_gml_search
; Purpose: Apply feature selection to find the best features to use for
; lung nodule detection. Use a GML classifier, a greedy search of the features
; and a leave an image out training/testing methodology to maximize the
; area under the FROC curve. Sequentially plot results as each new feature is
; selected.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/14/2003
;------------------------------------------------------------------------------
pro custom_gml_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, fn=fn, $
    maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, detectionlimit=detectionlimit

    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA_july242003.trn'

    ;fn = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'

    ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
    if(n_elements(fpt) eq 0) then fpt = 10.0

    numapriori = 300    ; The number of apriori values to use to sweep out the FROC curve
    numtoselect = 1    ; The number of features to select

    azrecord = dblarr(numtoselect)

    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
    limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    device, decompose=0
    window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Determine what the maximum possible FROC curve would be by using the
    ; correspondence of the candidates with the truth as the classifications
    ; of the candidate nodules.
    ;--------------------------------------------------------------------------
    tempclass = truth
    class = reform(tempclass, 1, n_elements(truth))
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
    print, perfect_tpf, perfect_fpi

    unselectedfeatures = indgen(n_elements(featurelabels))

    for q=0L,numtoselect-1 do begin

       best_new_az = 0
       best_new_feature = 0

       if(q le 7) then begin

         for i=0,n_elements(unselectedfeatures)-1 do begin

          trialfeature = unselectedfeatures[i]

          if(n_elements(selectedfeatures) eq 0) then begin
              candidatefeatures = [trialfeature]
          endif else begin
              candidatefeatures = [selectedfeatures, trialfeature]
          endelse

          if(q eq 0) then candidatefeatures = indgen(n_elements(featurelabels))
          if(q eq 1) then candidatefeatures = [0]
          if(q eq 2) then candidatefeatures = [20]
          if(q eq 3) then candidatefeatures = [28]
          if(q eq 4) then candidatefeatures = [34]
          if(q eq 5) then candidatefeatures = [20,28,34]
          if(q eq 6) then candidatefeatures = [20,28,34,0]
          if(q eq 7) then candidatefeatures = [1,17,14,7]

          if(n_elements(detectionlimit) eq 0) then begin
              traintest_gml, imagefilename, feature, truth, candidatefeatures, numapriori, class=class, cullmethod=cullmethod
          endif else begin
              traintest_gml, imagefilename, feature, truth, candidatefeatures, numapriori, class=class, detectionlimit=detectionlimit, cullmethod=cullmethod
          endelse
          score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

          simple_az, tpf, fpi, fpt, az

          intarray_to_string, candidatefeatures, thestring
          print, strtrim(string(az),2) + ': (' + thestring + ')'

          if(i eq 0) then begin
              best_new_az = az
              best_new_tpf = tpf
              best_new_fpi = fpi
              best_new_feature = trialfeature
          endif else begin
              if(az gt best_new_az) then begin
                 best_new_az = az
                 best_new_tpf = tpf
                 best_new_fpi = fpi
                 best_new_feature = trialfeature
              endif
          endelse

          ;for i=0L, n_elements(class[*,0])-1 do begin
          ;   plot_classification, feature, featurelabels, 0, 22, truth, class[i,*], $
          ;     useclassification=1, titlecomponent = 'N' + strtrim(string(n[i]),2)
          ;endfor

          class = class * 0

          if((i mod 5) eq 0) then wshow

          break

         endfor
       endif

       ;----------------------------------------------------------------------
       ; Add the best new feature to the set of selected features and remove
       ; it from the set of unselected features.
       ;----------------------------------------------------------------------
       if(n_elements(selectedfeatures) eq 0) then begin
         selectedfeatures = [best_new_feature]
       endif else begin
         selectedfeatures = [selectedfeatures, best_new_feature]
       endelse
       selectedfeatures = candidatefeatures

;     indx = where(unselectedfeatures ne best_new_feature, count)
;     if(count ne 0) then begin
;      newunselectedfeatures = unselectedfeatures[indx]
;      unselectedfeatures = newunselectedfeatures
;     endif

       intarray_to_string, selectedfeatures, thestring

       title = 'GML(' + strtrim(string(numapriori),2) + ',' + strtrim(string(fpt),2) + ') '
       labeltext = 'Custom (' + strtrim(string(best_new_az),2) + ') [' + thestring + '] ' + $
         '  ' ; + featurelabels[selectedfeatures[q]]

       if(q eq 0) then begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=0, title=title, maxfpitoplot
       endif else begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=1, color = colors[q], maxfpitoplot
       endelse
       ; plot_simple_froc, worst_tpf, worst_fpi, overplot=1

       if(q eq 0) then begin
         xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
       endif else begin
         loadct, 38, /silent
         xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
         loadct, 0, /silent
       endelse
       ;simple_az, imagefilename, truth, class, 10.0

       if(q eq 0) then begin
         loadct, 38, /silent
         oplot, [maxfpitoplot-1,100], [perfect_tpf,perfect_tpf], color = colors[0]
         xyouts, maxfpitoplot-1, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
         xyouts, maxfpitoplot-1, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
          strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
          color = colors[0]
         xyouts, maxfpitoplot-1, 0.03, alignment=1.0, base_fn, color=colors[0]
         loadct, 0, /silent
       endif

       azrecord[q] = best_new_az

       if(q eq 7) then break
    endfor

    ;--------------------------------------------------------------------------
    ; Score the initial candidate detection algorithm by selecting an integer
    ; number of candidates per image, where that integer goes from 0 to 12.
    ;--------------------------------------------------------------------------
    classify_by_candidate, imagefilename, candidate, 12, class=class
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
    simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az
    loadct, 0, /silent
    oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
    labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
    xyouts, 0.5, 0.97 - 0.03 * (q + 1), labeltext, alignment=0.0, color=0, /data
    if(n_elements(detectionlimit) ne 0) then begin
       xyouts, maxfpitoplot-1, 0.12, 'MaxD/I = ' + strtrim(string(fix(detectionlimit)),2), alignment=1.0, color=0, /data
    endif

    featurenumberrecord = selectedfeatures
    featurelabelrecord = featurelabels[featurenumberrecord]

end

;------------------------------------------------------------------------------
; Procedure: custom_knn_search
; Purpose: Apply feature selection to find the best features to use for
; lung nodule detection. Use a KNN classifier, a greedy search of the features
; and a leave an image out training/testing methodology to maximize the
; area under the FROC curve. Sequentially plot results as each new feature is
; selected.
; Name: Michael Heath, Eastman Kodak Company
; Date: 10/14/2003
;------------------------------------------------------------------------------
pro custom_knn_search, azrecord, featurenumberrecord, featurelabelrecord, maxfpitoplot, fn=fn, maxcandidatesperimage=maxcandidatesperimage, fpt=fpt, detectionlimit=detectionlimit

    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_60image.trn'
    ;fn = 'G:\detectionalysis\Debug\July242003\july242003_77image.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA_july242003.trn'

    ;fn = 'G:\detectionalysis\Debug\October072003\60image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\77image_october072003.trn'
    ;fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'

    kt = 5          ; This many neighbors must correspond to nodules for a candidate to be labeled as a nodule
    ; Use this as the upper limit of integration (average false positive threshold) of the FROC curve
    if(n_elements(fpt) eq 0) then fpt = 10.0
    n = 1 + indgen(300)    ; An array of the number of neighbors to consider to sweep out the FROC curve
    numtoselect = 10   ; The number of features to select

    azrecord = dblarr(numtoselect)

    colors = [0,2,4,6,8,11,12,14,15,16] * 14

    base_fn = strmid(fn, strpos(fn, '\',/reverse_search)+1, 100)

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    if(n_elements(maxcandidatesperimage) eq 0) then maxcandidatesperimage = max(candidate)+1
    limit_number_of_candidates, feature, label, candidate, imagefilename, correspondence, maxcandidatesperimage

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    device, decompose=0
    window, xsize=700, ysize=700, xpos=400, ypos=200, /free, retain=2

    ;--------------------------------------------------------------------------
    ; Determine what the maximum possible FROC curve would be by using the
    ; correspondence of the candidates with the truth as the classifications
    ; of the candidate nodules.
    ;--------------------------------------------------------------------------
    tempclass = truth
    class = reform(tempclass, 1, n_elements(truth))
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, perfect_tpf, perfect_fpi
    print, perfect_tpf, perfect_fpi

    unselectedfeatures = indgen(n_elements(featurelabels))


    for q=0L,numtoselect-1 do begin

       best_new_az = 0
       best_new_feature = 0

       if(q le 7) then begin

         for i=0,n_elements(unselectedfeatures)-1 do begin

          trialfeature = unselectedfeatures[i]

          if(n_elements(selectedfeatures) eq 0) then begin
              candidatefeatures = [trialfeature]
          endif else begin
              candidatefeatures = [selectedfeatures, trialfeature]
          endelse

          if(q eq 0) then candidatefeatures = indgen(n_elements(featurelabels))
          if(q eq 1) then candidatefeatures = [0]
          if(q eq 2) then candidatefeatures = [20]
          if(q eq 3) then candidatefeatures = [28]
          if(q eq 4) then candidatefeatures = [34]
          if(q eq 5) then candidatefeatures = [20,28,34]
          if(q eq 6) then candidatefeatures = [20,28,34,0]
          if(q eq 7) then candidatefeatures = [1,17,4,0]

          if(n_elements(detectionlimit) eq 0) then begin
              traintest_knn, imagefilename, feature, truth, candidatefeatures, n, kt, $
                 candidate=candidate, ngroups=ngroups, groups=groups, class=class, cullmethod=cullmethod
          endif else begin
              traintest_knn, imagefilename, feature, truth, candidatefeatures, n, kt, $
                 candidate=candidate, ngroups=ngroups, groups=groups, class=class, detectionlimit=detectionlimit, cullmethod=cullmethod
          endelse
          score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, tpf, fpi

          simple_az, tpf, fpi, fpt, az

          intarray_to_string, candidatefeatures, thestring
          print, strtrim(string(az),2) + ': (' + thestring + ')'

          if(i eq 0) then begin
              best_new_az = az
              best_new_tpf = tpf
              best_new_fpi = fpi
              best_new_feature = trialfeature
          endif else begin
              if(az gt best_new_az) then begin
                 best_new_az = az
                 best_new_tpf = tpf
                 best_new_fpi = fpi
                 best_new_feature = trialfeature
              endif
          endelse

          ;for i=0L, n_elements(class[*,0])-1 do begin
          ;   plot_classification, feature, featurelabels, 0, 22, truth, class[i,*], $
          ;     useclassification=1, titlecomponent = 'N' + strtrim(string(n[i]),2)
          ;endfor

          class = class * 0

          if((i mod 5) eq 0) then wshow

          break

         endfor
       endif

       ;----------------------------------------------------------------------
       ; Add the best new feature to the set of selected features and remove
       ; it from the set of unselected features.
       ;----------------------------------------------------------------------
       if(n_elements(selectedfeatures) eq 0) then begin
         selectedfeatures = [best_new_feature]
       endif else begin
         selectedfeatures = [selectedfeatures, best_new_feature]
       endelse

       selectedfeatures = candidatefeatures

       ;indx = where(unselectedfeatures ne best_new_feature, count)
       ;if(count ne 0) then begin
       ; newunselectedfeatures = unselectedfeatures[indx]
       ; unselectedfeatures = newunselectedfeatures
       ;endif

       intarray_to_string, selectedfeatures, thestring

       title = 'KNN(' + strtrim(string(kt),2) + ',' + strtrim(string(fpt),2) + ') '
       labeltext = 'Custom (' + strtrim(string(best_new_az),2) + ') [' + thestring + '] ' + $
         '  ' ; + featurelabels[selectedfeatures[q]]

       if(q eq 0) then begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=0, title=title, maxfpitoplot
       endif else begin
         plot_simple_froc, best_new_tpf, best_new_fpi, overplot=1, color = colors[q], maxfpitoplot
       endelse

       if(q eq 0) then begin
         xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
       endif else begin
         loadct, 38, /silent
         xyouts, 0.5, 0.97 - 0.03 * q, labeltext, alignment=0.0, color=colors[q], /data
         loadct, 0, /silent
       endelse

       if(q eq 0) then begin
         loadct, 38, /silent
         oplot, [maxfpitoplot-1,100], [perfect_tpf,perfect_tpf], color = colors[0]
         xyouts, maxfpitoplot-1, 0.09, alignment=1.0, 'N=' + strtrim(string(n_elements(truthnoduleid)),2), color=colors[0]
         xyouts, maxfpitoplot-1, 0.06, alignment=1.0, strtrim(string(perfect_tpf),2) + ', ' + $
          strtrim(string(double(n_elements(truth)) / double(n_elements(uniq(imagefilename)))),2), $
          color = colors[0]
         xyouts, maxfpitoplot-1, 0.03, alignment=1.0, base_fn, color=colors[0]
         loadct, 0, /silent
       endif

       azrecord[q] = best_new_az

       if(q eq 7) then break

    endfor

    ;--------------------------------------------------------------------------
    ; Score the initial candidate detection algorithm by selecting an integer
    ; number of candidates per image, where that integer goes from 0 to 12.
    ;--------------------------------------------------------------------------
    classify_by_candidate, imagefilename, candidate, 12, class=class
    score_froc, imagefilename, class, truthimagefilename, truthnoduleid, correspondence, best_new_tpf, best_new_fpi
    simple_az, best_new_tpf, best_new_fpi, fpt, best_new_az
    loadct, 0, /silent
    oplot, best_new_fpi, best_new_tpf, color=0, psym=4, symsize=0.75
    labeltext = 'ICD (' + strtrim(string(best_new_az),2) + ')'
    xyouts, 0.5, 0.97 - 0.03 * (q+1), labeltext, alignment=0.0, color=0, /data
    if(n_elements(detectionlimit) ne 0) then begin
       xyouts, maxfpitoplot-1, 0.12, 'MaxD/I = ' + strtrim(string(fix(detectionlimit)),2), alignment=1.0, color=0, /data
    endif

    featurenumberrecord = selectedfeatures
    featurelabelrecord = featurelabels[featurenumberrecord]
end

pro ti

    ;fn = 'G:\detectionalysis\Debug\July242003\training.out'
    ;fn = 'G:\detectionalysis\Debug\October072003\10072003training.txt'
    fn = 'G:\detectionalysis\Debug\October072003\setA_groundtruth\setA.trn'

    read_trn, fn, featurelabels, feature, label, candidate, imagefilename, correspondence, $
       truthimagefilename, truthnoduleid, ngroups, groups

    truth = intarr(n_elements(imagefilename))
    truth = truth * 0
    val = byte('P')
    indx = where(label eq val[0], count)
    print, count
    truth[indx] = 1
    classification = truth * 0

    ;for i=0L,n_elements(featurelabels)-2 do begin
    ;  for j=(i+1),n_elements(featurelabels)-1 do begin
    ;        plot_classification, feature, featurelabels, i, j, truth, classification
    ;  endfor
    ;endfor

    plot_classification, feature, featurelabels, 0, 6, truth, classification
end