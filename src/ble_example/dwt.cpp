#include "DWT.h"


wave_object::wave_object() {

    this->filtlength = 16;
    this->lpd_len = this->hpd_len = this->lpr_len = this->hpr_len = this->filtlength;
}


wt_object::wt_object(wave_object *wave, uint32_t siglength, uint8_t J) {
    int size;

    size = wave->filtlength;
    this->outlength = siglength + 2 * J * (size + 1);
    this->params = new float32_t[this->outlength];

    this->wave = wave;
    this->siglength = siglength;
    this->modwtsiglength = siglength;
    this->J = J;
    this->MaxIter = 8;

    if (siglength % 2 == 0) {
            this->even = 1;
    }
    else {
            this->even = 0;
    }

    this->cfftset = 0;
    this->lenlength = J + 2;
    this->output = &this->params[0];
    for (int i = 0; i < siglength + 2 * J * (size + 1); ++i) {
            this->params[i] = 0.0;
    }
}


void wt_object::dwt(const float32_t *inp) {
	int temp_len,iter,N,lp;
	int len_cA;
	float32_t orig[1024];
        float32_t orig2[1024];

	temp_len = this->siglength;
	this->length[J + 1] = this->siglength;
	this->outlength = 0;
	this->zpad = 0;
	//orig = new float32_t[temp_len];
	//orig2 = new float32_t[temp_len];

	for (int i = 0; i < this->siglength; ++i) {
		orig[i] = inp[i];
	}

	N = temp_len;
	lp = this->wave->lpd_len;

        int i = J;
        while (i > 0) {
                N = (int)ceil((float32_t)N / 2.0);
                this->length[i] = N;
                this->outlength += this->length[i];
                i--;
        }
        this->length[0] = this->length[1];
        this->outlength += this->length[0];
        N = this->outlength;

        for (iter = 0; iter < J; ++iter) {
                len_cA = this->length[J - iter];
                N -= len_cA;
                this->dwt1(orig, temp_len, orig2, this->params + N);

                temp_len = this->length[J - iter];
                if (iter == J - 1) {
                        for (i = 0; i < len_cA; ++i) {
                                this->params[i] = orig2[i];
                        }
                }
                else {
                        for (i = 0; i < len_cA; ++i) {
                                orig[i] = orig2[i];
                        }
                }
        }
}



void wt_object::dwt1(float32_t *sig, int len_sig, float32_t *cA, float32_t *cD) {
	int len_avg,D,lf;
	//float32_t *signal,*cA_undec;
        float32_t signal[1040];
        float32_t cA_undec[1055];
	len_avg = (this->wave->lpd_len + this->wave->hpd_len) / 2;

	D = 2;

        //signal = new float32_t[(len_sig + len_avg + (len_sig % 2))];

        len_sig = per_ext(sig, len_sig, len_avg / 2, signal);

        //cA_undec = new float32_t[(len_sig + len_avg + this->wave->lpd_len - 1)];

        arm_conv_f32(signal, len_sig + len_avg, this->wave->lpd, this->wave->lpd_len, cA_undec);

        downsamp(cA_undec + len_avg, len_sig, D, cA);

        arm_conv_f32(signal, len_sig + len_avg, this->wave->hpd, this->wave->hpd_len, cA_undec);

        downsamp(cA_undec + len_avg, len_sig, D, cD);
	
}


int wt_object::per_ext(float32_t *sig, int len, int a, float32_t *oup) {
	int i, len2;
	float32_t temp1;
	float32_t temp2;
	for (i = 0; i < len; ++i) {
		oup[a + i] = sig[i];
	}
	len2 = len;
	if ((len % 2) != 0) {
		len2 = len + 1;
		oup[a + len] = sig[len - 1];
	}
	for (i = 0; i < a; ++i) {
		temp1 = oup[a + i];
		temp2 = oup[a + len2 - 1 - i];
		oup[a - 1 - i] = temp2;
		oup[len2 + a + i] = temp1;
	}
	return len2;
}


int wt_object::downsamp(float32_t *x, int lenx, int M, float32_t *y) {
	int N, i;

	if (M < 0) {
		return -1;
	}
	if (M == 0) {
		for (i = 0; i < lenx; ++i) {
			y[i] = x[i];
		}
		return lenx;
	}

	N = (lenx - 1) / M + 1;

	for (i = 0; i < N; ++i) {
		y[i] = x[i*M];
	}

	return N;
}