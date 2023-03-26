#include "pch.h"
#include "Distribution.h"
using namespace std;


Hypergeom_distr::Hypergeom_distr(int _a, int _b, int _k)
	:a(_a)
	, b(_b)
	, k(_k)
	, last_calc(-1)
	, probs(new double[_k + 1])
{
	std::fill(probs, probs + k + 1, 0);
}

Hypergeom_distr::Hypergeom_distr(Hypergeom_distr& distr)
{
	a = distr.a;
	b = distr.b;
	k = distr.k;
	last_calc = distr.last_calc;
	if (distr.probs)
	{
		probs = new double[k+1];
		memcpy_s(probs, k+1, distr.probs, distr.k+1);
	}
	else
		probs = nullptr;
}

Hypergeom_distr::~Hypergeom_distr()
{
	if(probs)
	 delete[] probs; 
}

void Hypergeom_distr::set_param(int _a, int _b, int _k)
{
	if (_a < 1 || _b<1 || _k<1 || _k>_a || _k>_b)
		return;

	a = _a;
	b = _b;
	k = _k;
	last_calc = -1;
	if (probs)
		delete[] probs;
	probs = new double[k + 1];
}

void Hypergeom_distr::calc_all_probs()
{
	calc_probs_to(k);

	return;
}

void Hypergeom_distr::calc_next_prob()
{
	if (a < 1 || b < 1 || k < 1 || k > a || k > b||last_calc>k)
		return;

	if (last_calc > -1)
		probs[last_calc + 1] = probs[last_calc] * (a - last_calc) * (k - last_calc) / (last_calc + 1) / (b - k + last_calc + 1);
	else
	{
		int n = a + b, i = 0;
		probs[0] = 1;
		bool flg_1 = false, flg_2 = false;

		for (int j = 1; j < 2 * n - a - k + 1; ++j)
		{
			if (!flg_1)
				probs[0] *= j;
			else
				probs[0] *= j - b;

			if (!flg_2)
				probs[0] /= j;
			else
				probs[0] /= j - b + k;

			if (j == b)
				flg_1 = true;
			if (j == b - k)
				flg_2 = true;
		}
	}

	++last_calc;

	return;
}

double Hypergeom_distr::calc_probs_to(int n)
{
	if (a < 1 || b < 1 || k < 1 || k > a || k > b || last_calc>k)
		return -1;

	while (last_calc < n)
		calc_next_prob();

	return probs[n];
}

int* Hypergeom_bern::gen_sample()
{

	int a = h1.get_a();
	int b = h1.get_b();
	int k = h1.get_k();
	if (a < 1 || b < 1 || k < 1 || k > a || k > b)
		return nullptr;

	int* sim_freq = new int[k + 1];
	fill(sim_freq, sim_freq + k + 1, 0);
	int n = a + b, j = 0;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> rndm(0, 1);
	for (int i = 0; i < sample_sz; ++i)
	{
		j = 0;
		for (int i = 0; i < k; ++i)
			if (rndm(gen) < (a - j) / double(n - i))
				++j;
		++sim_freq[j];
	}
	return sim_freq;
}

int* Hypergeom_inv::gen_sample()
{
	int a = h1.get_a();
	int b = h1.get_b();
	int k = h1.get_k();
	const double* probs = h1.get_probs();
	double l, x;
	int i;
	if (a < 1 || b < 1 || k < 1 || k > a || k > b)
		return nullptr;


	int* sim_freq = new int[k + 1];
	fill(sim_freq, sim_freq + k + 1, 0);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> rndm(0, 1);
	for (int j = 0; j < sample_sz; ++j)
	{
		l = h1.calc_probs_to(0);
		i = 0;
		x = rndm(gen);
		while (x > l)
		{
			l += h1.calc_probs_to(i + 1);
			++i;
		}
		++sim_freq[i];
	}

	return sim_freq;
}

/*!
* \param generator Класс, генерирующий выборку
* \param h0 Распределения соответствующее H0
*/
Chi_sq::Chi_sq(Hypergeom_sample* generator, Hypergeom_distr& h0)
	: d_f(0), chi_sq(-1)
	, p_val(-1)
	, h_k(h0.get_k())
	, k(generator->get_k())
	, sample_sz(generator->get_n())
{ 
	h0.calc_all_probs();
	th_freq = new double[h_k + 1];

	for (int i = 0; i < h_k + 1; ++i)
		th_freq[i] = h0.get_probs()[i] * sample_sz;

	mod_freq = nullptr;

}

Chi_sq::~Chi_sq()
{
	if (th_freq)
		delete[] th_freq;
	if (mod_freq)
		delete[] mod_freq;
}

/*!
* \param generator Класс, генерирующий выборку
* \param h0 Распределения соответствующее H0
*/
void Chi_sq::setData(Hypergeom_sample* generator, Hypergeom_distr& h0)
{
	if (th_freq)
		delete[] th_freq;

	if (mod_freq)
		delete[] mod_freq;

	d_f = 0;
	chi_sq = -1;
	p_val = -1;
	h_k = h0.get_k();
	k = generator->get_k();
	sample_sz = generator->get_n();

	h0.calc_all_probs();
	th_freq = new double[h_k + 1];

	for (int i = 0; i < h_k + 1; ++i)
		th_freq[i] = h0.get_probs()[i]* sample_sz;


	mod_freq = generator->gen_sample();
}


double Chi_sq::calc_p_val()
{
	double th = 0, mod = 0, sum = 0;
	chi_sq = 0;
	d_f = 0;

	if (mod_freq == nullptr || th_freq == nullptr)
		return -1;

	for (int i = 0; i < max(k, h_k); ++i)
	{
		if (i <= k)
			mod += mod_freq[i];
		if (i <= h_k)
		{
			sum += th_freq[i];
			th += th_freq[i];
		}
		if (th > 5 && sample_sz - sum > 5)
		{
			chi_sq += (th - mod) * (th - mod) / th;
			++d_f;
			th = 0;
			mod = 0;
		}
	}
	if (th > 0)
		chi_sq += (th - mod) * (th - mod) / th;

	p_val = 1 - pChi(chi_sq, d_f);

	return p_val;
}

Hypergeom_sample::Hypergeom_sample(const Hypergeom_sample& smpl):h1(smpl.h1),sample_sz(smpl.sample_sz){}

int* Hypergeom_table::gen_sample()
{
	int a = h1.get_a();
	int b = h1.get_b();
	int k = h1.get_k();
	const double* probs = h1.get_probs();
	double l, x;
	int i;
	if (a < 1 || b < 1 || k < 1 || k > a || k > b)
		return nullptr;

	h1.calc_all_probs();
	
	int* sim_freq = new int[k + 1];
	fill(sim_freq, sim_freq + k + 1, 0);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> rndm(0, 1);
	for (int j = 0; j < sample_sz; ++j)
	{
		l = probs[0];
		i = 0;
		x = rndm(gen);
		while (x > l)
		{
			l += probs[i+1];
			++i;
		}
		++sim_freq[i];
	}

	return sim_freq;
}
