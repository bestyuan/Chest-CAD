


void detect_peaks(const IemTPlane<float> &img, const IemTPlane<byte> &mask)
{
    byte *mask_cp = mask[0], *in_cp = img[0];

    for(int r=2; r < img.rows() - 2; r++)
        for(int c=2; c < img.cols() - 2; c++, in_cp++)
            if(*mask_cp > 127)
            {
                local_peak = true;

                for(int dr=-1; dr <= 1; dr++)
                {
                    float *neigh_cp = img[r-dr]+c;
                    for(int dc=-1; dc <= 1; dc++, neigh_cp++)
                        if(*neigh_cp > *in_cp)
                            local_peak = false;
                }

                if(local_peak)
                    push_back(Peak(r, c, *in_cp));
            }
}
