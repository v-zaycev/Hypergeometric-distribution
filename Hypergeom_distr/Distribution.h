#pragma once
#include<iostream>
#include<vector>
#include<algorithm>
#include<random>
#include"PROBDIST.H"


/*! \mainpage Hypergeometric distribution 
 *
 * ����� �������, ����������� ������� ���������� � ������������������� ����������������, ������������ ������� ��� ��������� ������������� ���������� ��������, � ��������� �������� p-value
 � ������� �������� ��-������� ��� �������� ������� � �������������. <br> �������� ��� ������ �������������:
 * - \subpage ����� �������� �������
 * - \subpage ����� ��������� ��������
 * - \subpage �������� ����� �������� �������
 */

/*!
* \brief �����, ���������� ���������� � ������������������� �������������.
* 
* ������ ����� ������ � ���� ���������� � ���������� �������������������� �������������, � ����� ����������� ���������. ����������� ����� ��������� �� �����,
�� ��������� ��� ��� �����.
*/
class Hypergeom_distr
{
private:
	int a, b, k;
	int last_calc;
	double* probs;
public:
	/// �����������, ��������� ��������� � ��������� ����������� �������������
	Hypergeom_distr(int _a = 10, int _b = 10, int _k = 10);
	/// ����������� �����������
	Hypergeom_distr(Hypergeom_distr& distr);
	~Hypergeom_distr();
	/// �������, �������� ��� ��������� �������������
	void set_param(int _a, int _b, int _k);
	/// �������, ����������� ��� ����������� ��� ������� �������������
	void calc_all_probs();
	/// �������, ����������� ����������� ��� ������� ���������, ��� �������� ��� ��� �� ���������� 
	void calc_next_prob();
	/// �������, ����������� ��� ����������� ��� ��������� � 0 �� n-�� � ������������ ��������� ����������� �����������
	double calc_probs_to(int n);
	int get_a() { return a; }
	int get_b() { return b; }
	int get_k() { return k; }
	const double* get_probs() { return probs; };
};


/*!
* 
* \brief ���������� �����, ��������������� ��� ��������� ������� �������������������� �������������
* 
* 
* ������ ����� ��������� ������������ ������� ��� �������������������� ������������� � ��������� ����������� � ��������. ������ ����� ��� ����������, �����������
����� �������� �������, ����� ��������� �������� � ��������� ����� �������� �������. ����� ����� ���������s �������� ������ ���������� ������������ ������������� � ������ �������������. 
*/
class Hypergeom_sample	
{
protected:
	///������������ �������, ��������������� �������
//	int* sim_freq; 
	///�����, ���������� ��������� ������������� �������������
	Hypergeom_distr& h1;
	///������ �������
	int sample_sz; 
public:
	/// �����������, ��������� ������ ��� �������� ������������� � ������� �������
	/*!
	* \param _h1 �������������, ��� �������� ����� �������������� �������
	* \param _n ������ �������
	*/
	Hypergeom_sample(Hypergeom_distr& _h1, int _n = 1000) :sample_sz(_n), h1(_h1) {};
	/// ����������� �����������
	Hypergeom_sample(const Hypergeom_sample& smpl);
	virtual ~Hypergeom_sample() {};
	/// �������, ������������ �������
	virtual int* gen_sample()=0;
	/// �������, ����������� ��������� ����� ���������� ��� ������������ ������
	virtual Hypergeom_sample* copy()=0;	
	void set_sz(int sz) { sample_sz = sz; };
	int get_a() { return h1.get_a(); }
	int get_b() { return h1.get_b(); }
	int get_k() { return h1.get_k(); }
	int get_n() { return sample_sz; }
	/// ������� ������������ �����, ��������������� ������ �������������, ������� ���������� ����� 
	virtual int get_type() = 0;
};


/*!
*
* \brief ��������� ������ Hypergeom_sample, ����������� ����� �������� �������.
*
* 
* ������ ����� ��������� ������������ ������� ��� �������������������� ������������� � ��������� ����������� � �������� ������� �������� �������. ����� ����� ���������
 �������� ������ ���������� ������������ ������������� � ������ �������������.
*/
class Hypergeom_inv : public Hypergeom_sample
{
public:
	/// ����������� �����������
	Hypergeom_inv(const Hypergeom_inv& a) :Hypergeom_sample(a) {};
	/// �����������, ��������� ������ ��� �������� ������������� � ������� �������
	/*!
	* \param _h1 �������������, ��� �������� ����� �������������� �������
	* \param _n ������ �������
	*/
	Hypergeom_inv(Hypergeom_distr& h1, int n = 1000) :Hypergeom_sample(h1, n) {};
	/// �������, ������������ ������� ������� �������� �������
	virtual int* gen_sample();
	/// �������, ������������ �������� 0 - ��������������� ������ �������� �������
	virtual int get_type() { return 0; };
	virtual Hypergeom_sample* copy() { return new Hypergeom_inv(*this); };
	virtual ~Hypergeom_inv(){};
};


/*!
*
* \brief  ��������� ������ Hypergeom_sample, ����������� ��������� ����� �������� �������.
*
*
* ������ ����� ��������� ������������ ������� ��� �������������������� ������������� � ��������� ����������� � �������� ��������� ������� �������� �������. ����� ����� ���������
 �������� ������ ���������� ������������ ������������� � ������ �������������.
*/
class Hypergeom_table : public Hypergeom_sample
{
public:
	/// ����������� �����������
	Hypergeom_table(const Hypergeom_table& a) :Hypergeom_sample(a) {};
	/// �����������, ��������� ������ ��� �������� ������������� � ������� �������
	/*!
	* \param _h1 �������������, ��� �������� ����� �������������� �������
	* \param _n ������ �������
	*/
	Hypergeom_table(Hypergeom_distr &h1, int n = 1000) :Hypergeom_sample(h1, n) {};
	/// �������, ������������ ������� ��������� ������� �������� �������
	virtual int* gen_sample();
	/// �������, ������������ �������� 2 - ��������������� ���������� ������ �������� �������
	virtual int get_type() { return 2; };
	virtual Hypergeom_sample* copy() { return new Hypergeom_table(*this); };
	virtual ~Hypergeom_table() {};
};

/*!
*
* \brief ��������� ������ Hypergeom_sample, ������������ ��������� ��������.
*
*
* ������ ����� ��������� ������������ ������� ��� �������������������� ������������� � ��������� ����������� � �������� ������� ��������� ��������. ����� ����� ���������
 �������� ������ ���������� ������������ ������������� � ������ �������������.
*/
class Hypergeom_bern : public Hypergeom_sample
{
public:
	/// ����������� �����������
	Hypergeom_bern(const Hypergeom_inv& a) :Hypergeom_sample(a) {};
	/// �����������, ��������� ������ ��� �������� ������������� � ������� �������
	/*!
	* \param _h1 �������������, ��� �������� ����� �������������� �������
	* \param _n ������ �������
	*/
	Hypergeom_bern(Hypergeom_distr& h1, int n = 1000) :Hypergeom_sample(h1, n) {};
	/// �������, ������������ ������� ����������� ������� ��������� ��������
	virtual int* gen_sample();
	/// �������, ������������ �������� 1 - ��������������� ������ ��������� ��������
	virtual int get_type() { return 1; };
	virtual Hypergeom_sample* copy() { return new Hypergeom_bern(*this); };
	virtual ~Hypergeom_bern() {};
};

/*!

	\brief	�����, ����������� ���������� p-value � ������� �������� ��-�������

	������ ����� ��������� ������� �������� p-value ��� �������� �������� � �������. ��� ���������� � ������������ �������� �������� � ������ � ����� ���� �������� ��� �������������.
*/
class Chi_sq
{
private:
	/// �������� ���������� ��-�������
	double chi_sq;
	/// �������� p-value 
	double p_val; 
	/// ����� �������� �������
	int d_f;
	int	k; 
	int h_k;
	/// ������ �������
	int sample_sz;
	/// ������������ �������
	int* mod_freq;
	/// ������������� �������
	double* th_freq;


public:
	/// �����������, �������� ������� � �������������
	Chi_sq(Hypergeom_sample* generator, Hypergeom_distr& h0);
	~Chi_sq();

	/// �������, ��������� ����������� ���������� � ������������ � ��������� �������� � ��������������
	void setData(Hypergeom_sample* generator, Hypergeom_distr& h0);
	/// �������, ��������� �������� p-value ��� �������� ����� ������� � �������������.
	double calc_p_val();

	/// �������, ������������ �������� ���������� �
	double get_chi_sq() { return chi_sq; }
	/// �������, ������������ ����� �������� �������
	int get_d_f() { return d_f; }
	/// �������, ������������ �������� p-value
	double get_p_val() { return p_val; }
	/// �������, ������������ ������ �������
	int get_sample_sz() { return sample_sz; }
	/// �������, ������������ ������ ������������ ������
	const int* get_mod_freq() { return mod_freq; }
	/// �������, ������������ ������ ������������� ������
	const double* get_th_freq() { return th_freq; }
};

/** \example Main.cpp
 * 
 */