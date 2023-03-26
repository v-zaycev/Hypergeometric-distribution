
// Geom_distrDoc.h : interface of the CGeomdistrDoc class
//


#pragma once
#include "Distribution.h"

class CGeomdistrDoc : public CDocument
{
protected: // create from serialization only
	CGeomdistrDoc() noexcept;
	DECLARE_DYNCREATE(CGeomdistrDoc)

// Attributes
public:

// Operations	
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CGeomdistrDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

public:
	Hypergeom_distr h0;
	Hypergeom_distr h1;
	Hypergeom_sample* generator;
	Chi_sq calculator;

	double power; 
	double alpha; 
	int sample_sz; 
	int samples_nmb; 
	double p_lvls[101]; 
	double* power_n_dep; 
	int start_sz, steps_nmb, step_sz, power_n_samples_nmb;



	
	int display_cond=0;
	COLORREF th_distr = RGB(180, 180, 180);
	COLORREF mod_distr = RGB(100, 100, 100);
	COLORREF basic_line = RGB(0, 0, 0);
	COLORREF p_val_line = RGB(255, 0, 0);
	COLORREF power_n_line = RGB(0, 255, 0);
	COLORREF background = RGB(235, 235, 235);
	afx_msg void OnSetparameters();
	afx_msg void OnCalcPVal();
	afx_msg void OnRebuild();
	afx_msg void OnPvalparam();
	afx_msg void OnNPowerDep();
	afx_msg void OnPowerdependenceonn();

	void set_p_lvls(int _sample_sz, int _samples_nmb, double _alpha);
	/// Функция, задающая параметры, необходимые для получения зависимости мощности от размера выборки.
	void set_power_n(int _start_sz, int _steps_nmb, int _step_sz, int _power_n_samples_nmb, double _alpha);
	/// Функция, считающая значение мощности для заданной выборки из p-value.
	double calc_power(double* vals, int sz);
	/// Функция, генерирующая выборку из p-value для заданного H0 и H1.
	void gen_p_levels();
	/// Функция, получающая значения мощности для заданных H0 и H1.
	void power_n_dependance();
};
