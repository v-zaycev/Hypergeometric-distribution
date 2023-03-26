
// Geom_distrDoc.cpp : implementation of the CGeomdistrDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Geom_distr.h"
#endif

#include "Geom_distrDoc.h"
#include "Dial_basic.h"
#include "Dial_p_val.h"
#include "Dial_power_n.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGeomdistrDoc

IMPLEMENT_DYNCREATE(CGeomdistrDoc, CDocument)

BEGIN_MESSAGE_MAP(CGeomdistrDoc, CDocument)
	ON_COMMAND(ID_SETPARAMETERS, &CGeomdistrDoc::OnSetparameters)
	ON_COMMAND(ID_BUTTON_P_VAL, &CGeomdistrDoc::OnCalcPVal)
	ON_COMMAND(ID_REBUILD, &CGeomdistrDoc::OnRebuild)
	ON_COMMAND(ID_PVALPARAM, &CGeomdistrDoc::OnPvalparam)
	ON_COMMAND(ID_POWER_DEP, &CGeomdistrDoc::OnNPowerDep)
	ON_COMMAND(ID_POWERDEPENDENCEONN, &CGeomdistrDoc::OnPowerdependenceonn)
END_MESSAGE_MAP()


// CGeomdistrDoc construction/destruction

CGeomdistrDoc::CGeomdistrDoc() noexcept
	: power_n_dep(nullptr)
	, power(0)
	, alpha(0.05)
	, sample_sz(100)
	, samples_nmb(10'000)
	, start_sz(100)
	, steps_nmb(10)
	, step_sz(100)
	, power_n_samples_nmb(10'000) 
	, h0()
	, h1()
	, generator(new Hypergeom_inv(h1, 1000))
	, calculator(generator, h0)
{
	std::fill(p_lvls, p_lvls + 101, 0);
} 

CGeomdistrDoc::~CGeomdistrDoc()
{
	if(generator)
		delete generator;
	if (power_n_dep)
		delete[] power_n_dep;
}


BOOL CGeomdistrDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CGeomdistrDoc serialization

void CGeomdistrDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CGeomdistrDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CGeomdistrDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CGeomdistrDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGeomdistrDoc diagnostics

#ifdef _DEBUG
void CGeomdistrDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGeomdistrDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGeomdistrDoc commands


void CGeomdistrDoc::OnSetparameters()
{
	Dial_basic d(generator->get_type(), h1.get_a(), h1.get_b(), h1.get_k(), generator->get_n());


	if (d.DoModal() == IDOK)
	{
		if (!generator || generator->get_type() != d.func_type)
		{
			if (generator)
			{
				delete generator;
				generator = nullptr;
			}
			switch (d.func_type)
			{
			case 0:
			{
				generator = new Hypergeom_inv(h1, d.n_val);
				break;
			}
			case 1:
			{
				generator = new Hypergeom_bern(h1, d.n_val);
				break;
			}
			case 2:
			{
				generator = new Hypergeom_table(h1, d.n_val);
				break;
			}
			}
		}
		else
			generator->set_sz(d.n_val);

		h1.set_param(d.a_val, d.b_val, d.k_val);
		h0.set_param(d.a_val, d.b_val, d.k_val);
	}
}

void CGeomdistrDoc::OnCalcPVal()
{
	display_cond = 2;
	if (!generator)
		return;

	gen_p_levels();
	UpdateAllViews(0);
	return;
}

void CGeomdistrDoc::OnRebuild()
{
	display_cond = 1;
	if (!generator)
		return;

	calculator.setData(generator, h0);
	calculator.calc_p_val();
	UpdateAllViews(0);
	return;
}

void CGeomdistrDoc::OnPvalparam()
{
	Dial_p_val d(generator->get_type(), 
				 samples_nmb,
				 sample_sz, 
				 h1.get_a(),
				 h1.get_b(),
				 h1.get_k(),
				 h0.get_a(), 
				 h0.get_b(), 
				 h0.get_k(), 
				 alpha);


	if (d.DoModal() == IDOK)
	{
		if (!generator || generator->get_type() != d.type)
		{
			int n = 100;
			if (generator)
			{
				n = generator->get_n();
				delete generator;
				generator = nullptr;
			}
			switch (d.type)
			{
			case 0:
			{
				generator = new Hypergeom_inv(h1, n);
				break;
			}
			case 1:
			{
				generator = new Hypergeom_bern(h1, n);
				break;
			}
			case 2:
			{
				generator = new Hypergeom_table(h1, n);
				break;
			}
			}
		}
		h1.set_param(d.a, d.b, d.k);
		h0.set_param(d.h_a, d.h_b, d.h_k);
		set_p_lvls( d.sample_sz, d.samples_nmb, d.alpha);
	}
}

void CGeomdistrDoc::OnNPowerDep()
{
	display_cond = 3;
	if (!generator)
		generator = new Hypergeom_inv(h1);
	
	power_n_dependance();
	UpdateAllViews(0);
	return;
}

void CGeomdistrDoc::OnPowerdependenceonn()
{
	Dial_power_n d(generator->get_type(), 
				   h1.get_a(), 
				   h1.get_b(),
				   h1.get_k(),
			       h0.get_a(), 
		           h0.get_b(), 
				   h0.get_k(), 
				   start_sz,
				   step_sz,
				   steps_nmb,
				   power_n_samples_nmb,
				   alpha);


	if (d.DoModal() == IDOK)
	{
		if (!generator || generator->get_type() != d.type)
		{
			int n = 100;
			if (generator)
			{
				n = generator->get_n();
				delete generator;
				generator = nullptr;
			}
			switch (d.type)
			{
			case 0:
			{
				generator = new Hypergeom_inv(h1, n);
				break;
			}
			case 1:
			{
				generator = new Hypergeom_bern(h1, n);
				break;
			}
			case 2:
			{
				generator = new Hypergeom_table(h1, n);
				break;
			}
			}
		}
		
		h1.set_param(d.a, d.b, d.k);
		h0.set_param(d.h_a, d.h_b, d.h_k);
		set_power_n(d.init_sz, d.steps_nmb, d.step_sz, d.sample_sz, d.alpha);
	}
}

/*!
* \param _sample_sz Размер выборки, для которой получается значение p-value
* \param _samples_nmb Количество выборок, для которых считается значение p-value
* \param _alpha Заданный уровень значимости
*/
void CGeomdistrDoc::set_p_lvls(int _sample_sz, int _samples_nmb, double _alpha)
{
	sample_sz = _sample_sz;
	samples_nmb = _samples_nmb;
	alpha = _alpha;
	std::fill(p_lvls, p_lvls + 101, 0);
}

/*!
* \param _start_sz Минимальный размер выборки
* \param _steps_nmb Количество различных размеров выборок
* \param _step_sz Отличие размера следующей выборки от предыдущей
* \param _power_n_samples_nmb Количество значений p-value необходимых для получения мощности
* \param _alpha Заданный уровень значимости
*/
void CGeomdistrDoc::set_power_n(int _start_sz, int _steps_nmb, int _step_sz, int _power_n_samples_nmb, double _alpha)
{
	start_sz = _start_sz;
	steps_nmb = _steps_nmb;
	step_sz = _step_sz;
	power_n_samples_nmb = _power_n_samples_nmb;
	alpha = _alpha;
}

/*!
* \param vals Выборка из p-value
* \param sz Размер выборки
*/
double CGeomdistrDoc::calc_power(double* vals, int sz)
{
	int k = 0;

	for (int i = 0; i < sz; ++i)
		if (vals[i] < alpha)
			++k;

	return double(k) / sz;
}

/*!
* \param smpl Класс, генерирующий выборку с заданными H1 и методом
* \param sz Распределения соответствующее H0
*/
void CGeomdistrDoc::gen_p_levels()
{
//	Hypergeom_sample* gen = generator->copy();
	generator->set_sz(sample_sz);

	double* p_vals = new double[samples_nmb];

	h0.calc_all_probs();
	for (int i = 0; i < samples_nmb; ++i)
	{
		calculator.setData(generator, h0);
		p_vals[i] = calculator.calc_p_val();
	}


	std::sort(p_vals, p_vals + samples_nmb);
	int j = 0, i = 1;
	while (j < samples_nmb - 1)
		if (p_vals[j + 1] < double(i) / 100)
			++j;
		else
		{
			p_lvls[i] = double(j) / samples_nmb;
			++i;
		}
	for (; i < 101; ++i)
		p_lvls[i] = 1;

	power = calc_power(p_vals, samples_nmb);

	delete[] p_vals;

	return;
}

/*!
* \param smpl Класс, генерирующий выборку с заданными H1 и методом
* \param sz Распределения соответствующее H0
*/
void CGeomdistrDoc::power_n_dependance()
{
	Hypergeom_sample* gen = generator->copy();

	if (power_n_dep)
		delete[] power_n_dep;
	power_n_dep = new double[steps_nmb];

	double* p_vals = new double[power_n_samples_nmb];
	h0.calc_all_probs();

	for (int k = 0; k < steps_nmb; ++k)
	{
		gen->set_sz(start_sz + k * step_sz);
		for (int i = 0; i < power_n_samples_nmb; ++i)
		{
			calculator.setData(gen, h0);
			p_vals[i] = calculator.calc_p_val();
		}
		power_n_dep[k] = calc_power(p_vals, power_n_samples_nmb);
	}

	delete gen;
	delete[] p_vals;

	return;
}