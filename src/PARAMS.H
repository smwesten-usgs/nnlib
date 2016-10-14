#ifndef __CPARAMS_H__
#define __CPARAMS_H__

	struct _params {
		int m_TrainFlags;
		int m_AImaxhid;
		int m_goodness;
		int m_autosave;
		int m_seed;
		int m_eon;

		long m_cnt_max;

		double m_hiddegrad;
		double m_errtol;
		double m_goodrsq;
		double m_signinc;
		double m_nosigninc;
		double m_alpha;
		double m_theta;
		double m_randz;
		double m_inrandzdiv;
		double m_tol;
		double m_learning_rate;
		double m_Hlearning_rate;
		double m_tlearning_rate;
		double m_inoutlearn;
	};

	typedef struct _params PARAMS;
	PARAMS *CreateParams( );
    int ImportParams (FILE *fd, PARAMS *pM, int *ExportVersion);
	void DumpParams(PARAMS *pM, FILE *fd );

#endif // __CPARAMS_H__

