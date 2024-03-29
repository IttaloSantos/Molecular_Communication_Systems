#include <bits/stdc++.h>

using namespace std;

#define SCALE 1
#define INV_SCALE 1/SCALE
#define QTD_DIFFUSIONS 4
#define DIM_X 15
#define DIM_Y 5
#define DIM_Z 3
#define ALPHA 0.01
#define PI 3.14159265359
#define R 8.314472 // [N.m/(g.mol.K)] (Gas constant) Zaki Ahmad, in Principles of Corrosion Engineering and Corrosion Control, 2006
#define T 298.16 // [K] Ambient temperature
#define n_NA 1 // Sodium charge
#define n_CA 2 // Calcium charge
#define F 96485.3329 // [s.A/mol] Faraday's constant
#define e_0 1.6e-19 // [C] Elementary charge
#define c_pulse 0.5
#define low_threshold 0.0
#define high_threshold 0.03

double conc, tx_conc, diameter_cell = 5, deltaamp;

double NKATP_n = 5, NKATP_v = 121; // NKATP_v s^-1 - Clarke, Apell e Lauger (1989)
double a_K = 60.0, b_K = 1e-9, a_N = 8e-10, b_N = 8e-10; // M^-1 * cm * s^-1 // Clarke, Apell e Lauger (1989)
double r_K = diameter_cell/2, d_K = 4e-9; // 45e-9 m
double ATP_THRESHOLD = 0; // MODIFICAR!!!!!!!!!!!!!!!!!!!
double lambda = 2e-3; // uS / cm^2

// #### GAP JUNCTIONS PROBABILITIES FOR ASTROCYTES #### //
double phl[50] = {3.33333333333e-01,9.51756626745e-02,2.71812917035e-02,7.76661124715e-03,2.22288659849e-03,6.39921679991e-04,1.87410416804e-04,5.81750667195e-05,2.17596143238e-05,1.1436923158e-05,7.88454209682e-06,7.43738619183e-06,7.37970057786e-06,7.29603316347e-06,7.27478942971e-06,7.26006289992e-06,7.26084787208e-06,7.26080132601e-06,7.26061054996e-06,7.26081361742e-06,7.26079620991e-06,7.26072365567e-06,7.26058079345e-06,7.26074725419e-06,7.26087576894e-06,7.26073008288e-06,7.26061194028e-06,7.26074336727e-06,7.26101528686e-06,7.26081974085e-06,7.26091667847e-06,7.26058059059e-06,7.26084014577e-06,7.2610063969e-06,7.26069065682e-06,7.26083092741e-06,7.26076153595e-06,7.26071756287e-06,7.26092535023e-06,7.26076421324e-06,7.26060026219e-06,7.26075209967e-06,7.26093367537e-06,7.26073986493e-06,7.26039032094e-06,7.26091299989e-06,7.26077756319e-06,7.26071491915e-06,7.2607710224e-06,7.26082337127e-06};
double plh[50] = {0.333333333333,0.706523083189,0.825908352326,0.86117728508,0.871356970354,0.874272895353,0.875107602476,0.875346523333,0.875414979813,0.875433066086,0.875439533589,0.875437420808,0.875439605921,0.875443525702,0.875437685437,0.875440602592,0.875441218644,0.875440938251,0.875438148928,0.875441400815,0.875441147657,0.875440025483,0.875437801975,0.875440355641,0.875442338151,0.875440081279,0.87543825197,0.875440285023,0.87544449347,0.875441466845,0.875442967173,0.875437765288,0.875441782611,0.875444355802,0.875439468868,0.875441639938,0.875440565916,0.875439885313,0.875443101387,0.875440607355,0.875438069767,0.875440419864,0.875443230241,0.875440230498,0.875434820356,0.875442910232,0.875440813981,0.875439844395,0.875440712745,0.875441522986};
double phh[50] = {0.333333333333,0.198301254137,0.14691035597,0.131056103673,0.126420143048,0.125087182967,0.124704987107,0.1245953016,0.124563260573,0.124555496991,0.124552581869,0.124555141806,0.124553014379,0.124549178265,0.124555039774,0.124552137345,0.124551520508,0.124551800947,0.124554590462,0.124551338371,0.124551591547,0.124552713793,0.124554937445,0.124552383612,0.124550400973,0.124552657991,0.124554487418,0.124552454233,0.124548245514,0.124551272335,0.12454977191,0.124554974131,0.124550956549,0.124548383192,0.124553270441,0.124551099232,0.124552173322,0.124552853969,0.124549637687,0.124552131881,0.124554669633,0.124552319384,0.124549508825,0.124552508762,0.124557919254,0.124549828855,0.124551925241,0.12455289489,0.124552026484,0.124551216191};

// Classe para representar todas as variáveis da célula (Ex.: astrócito)
class Cell {
public:
	map<string, double> parameters;
	int id;

	Cell() {
		/*
			0v1: taxa de liberação de Cálcio do Tx
			1Y:
			2vin: o fluxo de calcio que parte do espaço extracelular, por meio da membrana do astrócito, até o interior do citosol
			3VM2: o fluxo máximo de íons de cálcio fora da bomba (???)
			4C: Concentração de Cálcio no citosol
			5n: Coeficiente de Hill (2.02)
			6K2:
			10kf: constante que determina a liberação de cálcio do RE para o citosol
			22D: coneficiente de difusão
			23l: volume da célula
			24K: taxa máxima da ativação do receptor (Nakano, 2010; Eq. 3)
			25ka: taxa máxima da ativação do receptor (2.5) (Nakano, 2010; Eq. 3)
			26m: Coeficiente de Hill (2.2)
			27phh - 29plh: probabilidades das gap junctions
		*/

		double v1 = 100*SCALE; parameters["v1"] = v1;
		double Y = 0*SCALE; parameters["Y"] = Y;
		double vin = 0.05*SCALE; parameters["vin"] = vin;
		double VM2 = 15*SCALE; parameters["VM2"] = VM2;
		double C = 0.1*SCALE; parameters["C"] = C;
		double n = 2.02*SCALE; parameters["n"] = n;
		double K2 = 0.1*SCALE; parameters["K2"] = K2;
		double VM3 = 40*SCALE; parameters["VM3"] = VM3; // Por que nao 40, como diz no artigo?
		double ko = 0.5*SCALE; parameters["ko"] = ko;
		double ER = 1.5*SCALE; parameters["ER"] = ER;
		double kf = 0.5*SCALE; parameters["kf"] = kf;
		double kp = 0.3*SCALE; parameters["kp"] = kp;
		double kdeg = 0.08*SCALE; parameters["kdeg"] = kdeg;
		double vp = 0.05*SCALE; parameters["vp"] = vp;
		double kcaaa = 0.15*SCALE; parameters["kcaaa"] = kcaaa;
		double kcai = 0.15*SCALE; parameters["kcai"] = kcai;
		double kip3 = 0.1*SCALE; parameters["kip3"] = kip3;
		double IP3 = 0.1*SCALE; parameters["IP3"] = IP3;
		double q = 4*SCALE; parameters["q"] = q;
		double W = 0*SCALE; parameters["W"] = W;
		double A = 0*SCALE; parameters["A"] = A;
		double kia = 0.5*SCALE; parameters["kia"] = kia;
		double D = 350 * (4 * PI * diameter_cell / 3.0)/*122500*/; parameters["D"] = D;
		double l = PI *SCALE * pow(diameter_cell / 2, 2); parameters["l"] = l;
		double K = 0.0006 *SCALE; parameters["K"] = K;
		double ka = 2.5*SCALE; parameters["ka"] = ka;
		double m = 2.2*SCALE; parameters["m"] = m;
		parameters["phl"] = phl[0];
		parameters["plh"] = plh[0];
		parameters["phh"] = phh[0];

		double Na_i = SCALE*15000;  parameters["Na_i"] = Na_i; //Langer3, Chatton 2016 [uM] - Colocar uma relação
		double Na_o = SCALE*150000/(DIM_X * DIM_Y * DIM_Z);  parameters["Na_o"] = Na_o; //chatton 2016 [uM]
		double NaD = (4/3)*PI*diameter_cell*600; parameters["NaD"] = NaD; 
		double C_o = SCALE*2300/(DIM_X * DIM_Y * DIM_Z); parameters["C_o"] = C_o; //Kirischuk 1997 [uM]
		//double K_i = 0; parameters["K_i"] = K_i; // A definir
		//double K_o = 0; parameters["K_o"] = K_o; // A definir
		double V_NCX = -SCALE*80; parameters["V_NCX"] = V_NCX; //Verify this value later - Koenigsberger, 2004-->-40/Kirischuk 2012-->-80 [mV]
		double Vm = SCALE*R*T/(n_CA*F)*log(C_o/C)*1000; parameters["Vm"] = Vm; // [mV] VERIFICAR!!!! Nernst Eq./Eq (3) from  Koenigsberger, 2004
		double DDC_Na = -SCALE*115000; parameters["DDC_Na"] = DDC_Na; // Kirischuk 2012 [uM]
		double G_NCX = SCALE*0.00316; parameters["G_NCX"] = G_NCX; // Koenigsberger, 2004 [uM.m.V^(-1).S^(-1)]
		double C_NCX = SCALE*0.5; parameters["C_NCX"] = C_NCX; // Barros, 2015 [uM]
		// EQUAÇÕES PARALELAS
		double K_mCaAct = SCALE*0.394; parameters["K_mCaAct"] = K_mCaAct; // Weber, 2001 & Brazhe 2018 [uM]
		double n_Hill = SCALE*2; parameters["n_Hill"] = n_Hill; // Weber, 2001 & Brazhe 2018
		double J_max = SCALE*25; parameters["J_max"] = J_max; // Brazhe 2018 [A/F] 25-60
		double eta = SCALE*0.35; parameters["eta"] = eta; // Weber, 2001 & Brazhe 2018
		double k_sat = SCALE*0.27; parameters["k_sat"] = k_sat; // Weber, 2001 & Brazhe 2018
		double K_mNao = SCALE*87500; parameters["K_mNao"] = K_mNao; // Weber, 2001 & Brazhe 2018 [uM]
		double K_mNai = SCALE*12300; parameters["K_mNai"] = K_mNai; // Weber, 2001 & Brazhe 2018 [uM]
		double K_mCao = SCALE*1300; parameters["K_mCao"] = K_mCao; // Weber, 2001 & Brazhe 2018 [uM]
		double K_mCai = SCALE*3.6; parameters["K_mCai"] = K_mCai; // Weber, 2001 & Brazhe 2018 [uM]
		double H_Na = SCALE*2; parameters["H_Na"] = H_Na; // Matsuoka 1996 & Brazhe 2018 ou 1 ***********?????????????
		double H_Ca = SCALE*1.2; parameters["H_Ca"] = H_Ca; // Matsuoka 1996 & Brazhe 2018 ou 1.1 ***********?????????????
		double K_Na = SCALE*28000; parameters["K_Na"] = K_Na; // Matsuoka 1996 & Brazhe 2018 ou 1 [uM] ***********?????????????
		double K_Ca = SCALE*0.1; parameters["K_Ca"] = K_Ca; // Matsuoka 1996 & Brazhe 2018 ou 1.4 [uM] ***********?????????????
		double tau_0 = SCALE*0.1; parameters["tau_0"] = tau_0; // Matsuoka 1996 & Brazhe 2018 [s] ***********?????????????
		double H_tau = SCALE*0.5; parameters["H_tau"] = H_tau; // Matsuoka 1996 & Brazhe 2018 ***********?????????????
		double K_tau = SCALE*0.5; parameters["K_tau"] = K_tau; // Matsuoka 1996 & Brazhe 2018 ***********?????????????
		double tau_Ca = SCALE*0.001; parameters["tau_Ca"] = tau_Ca; // Brazhe 2018 ***********?????????????
		double C_rest = SCALE*0.073; parameters["C_rest"] = C_rest; // Kirischuk 2012 [uM] 

		// NKATP
		double K_i = SCALE * 140000; parameters["K_i"] = K_i; // uM - Vogt (2019)
		double K_o = SCALE * 4000; parameters["K_o"] = K_o; // uM - Wang (2012)
		double c_K_i = SCALE * 25000; parameters["c_K_i"] = c_K_i; // uM - Clarke, Apell e Lauger (1989)
		double c_K_o = SCALE * 25000; parameters["c_K_o"] = c_K_o; // uM - Clarke, Apell e Lauger (1989)
		double c_Na_i = SCALE * 125000; parameters["c_Na_i"] = c_Na_i; // uM - Clarke, Apell e Lauger (1989)
		double c_Na_o = SCALE * 125000; parameters["c_Na_o"] = c_Na_o; // uM - Clarke, Apell e Lauger (1989)
		double J_Np = -3 * NKATP_n * NKATP_v; parameters["J_Np"] = J_Np; // uM/s - Clarke, Apell e Lauger (1989)
		double J_Kp = 2 * NKATP_n * NKATP_v; parameters["J_Kp"] = J_Kp; // uM/s - Clarke, Apell e Lauger (1989)
		double c_val = SCALE*50e-9; parameters["c_val"] = c_val; // Variar de 0 a 50 nM - Primeiro teste: 25e-3 - Segundo teste: 35e-3
		double P_K = a_K * c_val + b_K; parameters["P_K"] = P_K; // Clarke, Apell e Lauger (1989)
		double P_N = a_N * c_val + b_N; parameters["P_N"] = P_N; // Clarke, Apell e Lauger (1989)
		double A_vesicles = 4 * PI * (r_K + d_K); parameters["A_vesicles"] = A_vesicles;
		double ATP = SCALE * 100; parameters["ATP"] = ATP; // uM - Wang, 2012
		double ADP = SCALE * 0; parameters["ADP"] = ADP; // uM - ENCONTRAR BASELINE !!!!!!!!!!!!

		// CP
		double G_cp = SCALE * 0.9; parameters["G_cp"] = G_cp; // uM.s^-1
		double K_cp = SCALE * 0.1; parameters["G_cp"] = G_cp; // s

		// Modulation and Demodulation
		double C_variation = SCALE*0; parameters["C_variation"] = C_variation;
		double tx_rx_reactions = 0; parameters["tx_rx_reactions"] = tx_rx_reactions;

	}

	void setId(int ID) {
		id = ID;
		parameters["id"] = ID;
	}
};

class Network {
public:
	int NC; // Number of cells
	Cell tecido[DIM_Y][DIM_X][DIM_Z];
	list<int> *connect;
	map <string,vector<double>> rxtx_concentration;

	Network() {
		NC = DIM_X * DIM_Y * DIM_Z;
		connect = new list<int>[NC];

		//SETTING VALUES
		vector<double> tx; rxtx_concentration["tx"] = tx;
		vector<double> rx; rxtx_concentration["rx"] = rx;

		int id_cont = 0;

		for (int k = 0; k < DIM_Z; k++) {
			for (int i = 0; i < DIM_Y; i++) {
				for (int j = 0; j < DIM_X; j++) {
					Cell celula;
					celula.setId(id_cont);
					tecido[i][j][k] = celula;
					id_cont++;
				}
			}
		}
	}

	int getId(int x, int y, int z) {
		return tecido[y][x][z].parameters["id"];
	}

	double get(int x, int y, int z, string parameter) {
		return tecido[y][x][z].parameters[parameter];
	}

	double get(int id, string parameter) {
		int x = id % DIM_X;
		int y = (id / DIM_X) % DIM_Y;
		int z = id / (DIM_X * DIM_Y);
		return tecido[y][x][z].parameters[parameter];
	}

	void set(int x, int y, int z, string parameter, double value) {
		tecido[y][x][z].parameters[parameter] = value;
	}

	void set(int id, string parameter, double value) {
		int x = id % DIM_X;
		int y = (id / DIM_X) % DIM_Y;
		int z = id / (DIM_X * DIM_Y);
		tecido[y][x][z].parameters[parameter] = value;
	}

	void accumulate(int x, int y, int z, string parameter, double add) {
		tecido[y][x][z].parameters[parameter] += add;
	}
	
	void update_parameters(int x, int y, int z, string parameter) {
		if (parameter == "Vm"){
			tecido[y][x][z].parameters[parameter] = R*T/(n_CA*F)*log(tecido[y][x][z].parameters["C_o"]/tecido[y][x][z].parameters["C"])*1000; // ALTERAR!!!
		}
	}

	void accumulate(int id, string parameter, double add) {
		int x = id % DIM_X;
		int y = (id / DIM_X) % DIM_Y;
		int z = id / (DIM_X * DIM_Y);
		tecido[y][x][z].parameters[parameter] += add;
	}

	void changeSignal(int x, int y, int z, string parameter) {
		tecido[y][x][z].parameters[parameter] *= -1;
	}

	void printTissue() {
		cout << endl;
		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				cout << fixed << setprecision(2) << get(i, j, trunc(DIM_Z / 2), "C") << ":";
				cout << fixed << setprecision(2) << get(i, j, trunc(DIM_Z / 2), "C_o") << "  ";

			}
			cout << endl;
		}
		cout << endl << endl;
		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				cout << fixed << setprecision(2) << get(i, j, trunc(DIM_Z / 2), "Na_i") << ":";
				cout << fixed << setprecision(2) << get(i, j, trunc(DIM_Z / 2), "Na_o") << "  ";

			}
			cout << endl;
		}
	}

	void writeFileHeader(ofstream &file){
		for (int i = 0; i < DIM_Y; i++) {
		  for (int j = 0; j < DIM_X; j++) {
			if(i == DIM_Y-1 && j == DIM_X-1)
			  file << i << "_" << j;
			else
			  file <<  i << "_" << j << ",";
		  }
		}
		file << endl;
	}

	void printTissuef(ofstream &file, int time_int){
		file << time_int << ",";
		for (int i = 0; i < DIM_Y; i++) {
		  for (int j = 0; j < DIM_X; j++) {
			if(i == DIM_Y-1 && j == DIM_X-1)
			  file << get(j, i, trunc(DIM_Z / 2), "C");
			else
			  file <<  get(j, i, trunc(DIM_Z / 2), "C") << ",";
		  }
		}
		file << endl;
	}

	void regularDegree() {
		for (int x = 0; x < DIM_X; x++) {
			for (int y = 0; y < DIM_Y; y++) {
				for (int z = 0; z < DIM_Z; z++) {
					// x + 1
					if (y + 1 < DIM_Y) {
						connect[getId(x, y, z)].push_back(getId(x, y + 1, z));
					} else {
						connect[getId(x, y, z)].push_back(-1);
					}
					// x - 1
					if (y - 1 >= 0) {
						connect[getId(x, y, z)].push_back(getId(x, y - 1, z));
					} else {
						connect[getId(x, y, z)].push_back(-1);
					}
					// y + 1
					if (x + 1 < DIM_X) {
						connect[getId(x, y, z)].push_back(getId(x + 1, y, z));
					} else {
						connect[getId(x, y, z)].push_back(-1);
					}
					// y - 1
					if (x - 1 >= 0) {
						connect[getId(x, y, z)].push_back(getId(x - 1, y, z));
					} else {
						connect[getId(x, y, z)].push_back(-1);
					}
					// z + 1
					if (z + 1 < DIM_Z) {
						connect[getId(x, y, z)].push_back(getId(x, y, z + 1));
					} else {
						connect[getId(x, y, z)].push_back(-1);
					}
					// z - 1
					if (z - 1 >= 0) {
						connect[getId(x, y, z)].push_back(getId(x, y, z - 1));
					} else {
						connect[getId(x, y, z)].push_back(-1);
					}
				}
			}
		}

		// IMPRIMINDO AS CONEXÕES DE CADA CÉLULA
		// list<int>::iterator it;

		// for (int v = 0; v < NC; v++) {
		// 	cout << "Célula " << v << ": ";
		// 	for (it = connect[v].begin(); it != connect[v].end(); it++){
		// 		cout << *it << " ";
		// 	}
		// 	cout << endl;
		// }
	}

	void linkRadius(int radius) {
		for (int x = 0; x < DIM_X; x++) {
			for (int y = 0; y < DIM_Y; y++) {
				for (int z = 0; z < DIM_Z; z++) {
					for (int r = 1; r <= radius; r++) {
						// x + r
						if (y + r < DIM_Y) {
							connect[getId(x, y, z)].push_back(getId(x, y + r, z));
						} else {
							connect[getId(x, y, z)].push_back(-1);
						}
						// x - r
						if (y - r >= 0) {
							connect[getId(x, y, z)].push_back(getId(x, y - r, z));
						} else {
							connect[getId(x, y, z)].push_back(-1);
						}
						// y + r
						if (x + r < DIM_X) {
							connect[getId(x, y, z)].push_back(getId(x + r, y, z));
						} else {
							connect[getId(x, y, z)].push_back(-1);
						}
						// y - r
						if (x - r >= 0) {
							connect[getId(x, y, z)].push_back(getId(x - r, y, z));
						} else {
							connect[getId(x, y, z)].push_back(-1);
						}
						// z + r
						if (z + r < DIM_Z) {
							connect[getId(x, y, z)].push_back(getId(x, y, z + r));
						} else {
							connect[getId(x, y, z)].push_back(-1);
						}
						// z - r
						if (z - r >= 0) {
							connect[getId(x, y, z)].push_back(getId(x, y, z - r));
						} else {
							connect[getId(x, y, z)].push_back(-1);
						}
					}
				}
			}
		}

		// IMPRIMINDO AS CONEXÕES DE CADA CÉLULA
		list<int>::iterator it;

		for (int v = 0; v < NC; v++) {
			cout << "Célula " << v << ": ";
			for (it = connect[v].begin(); it != connect[v].end(); it++){
				cout << *it << " ";
			}
			cout << endl;
		}
	}

	vector<int> getConnections(int id) {
		vector<int> connections;
		list<int>::iterator it;

		for (it = connect[id].begin(); it != connect[id].end(); it++){
			connections.push_back(*it);
		}

		return connections;
	}

	int numberConnections() {
		return connect[0].size();
	}

	// CSK Modulation
	int modulation(int x, int y, int z)
	{
		if ( tecido[y][x][z].parameters["C_variation"] >= high_threshold )
		{
			return 1;
		}
		else if ( tecido[y][x][z].parameters["C_variation"] >= low_threshold && tecido[y][x][z].parameters["C_variation"] < high_threshold )
		{
			return 0;
		}
		return -1;
	}

	int modulation(double c)
	{
		if (c >= high_threshold)
		{
			return 1;
		}else if (c >= low_threshold && c < high_threshold)
		{
			return 0;
		}
		return -1;
	}

	// Demodulation
	void demodulation(int* Rx_states, int Tx_id, int Rx_id, double total_reactions, int time_size)
	{
		double Tx_p = get(Tx_id, "tx_rx_reactions") / total_reactions;
		double Rx_p = get(Rx_id, "tx_rx_reactions") / total_reactions;
		double CIR[time_size]; // Channel Impulse Response
		
		int    alpha = static_cast<int>( *max_element(rxtx_concentration["tx"].begin(), rxtx_concentration["tx"].end()) / ALPHA );

		// cout << "aplha = " << alpha << endl;

		// double alpha = static_cast<int>( ( max_element(rxtx_concentration["rx"].begin, rxtx_concentration["rx"].end) + max_element(rxtx_concentration["tx"].begin, rxtx_concentration["tx"].end) ) / (2 * ALPHA);

		default_random_engine generator;

		gamma_distribution<> distribution(alpha, Tx_p);

		// cout << setprecision(5) << rx_reactions / total_reactions * Tx_p[j] << ",";

		for (int k = 0; k < time_size; k++)
		{	
			CIR[k] = distribution(generator);
			// cout << CIR[k] << " ";
			
			Rx_states[k] = modulation(CIR[k]*rxtx_concentration["tx"][k]);// + CIR[i][1][k]);
		}
		// cout << endl;
	}

	double conditional_accumulate(int* first, int size, int value)
	{
		double sum = 0;

		for (int i = 0; i < size; i++)
		{
			if(*(first + i) == value) sum += 1;
			// cout << "bit: " << *first << endl;
		}

		return sum;
	}

	double conditional_accumulate(vector<int>::iterator first, int size, int value)
	{
		double sum = 0;

		for (int i = 0; i < size; i++)
		{
			if(*(first + i) == value) sum += 1;
			// cout << "bit: " << *first << endl;
		}

		return sum;
	}

};

class Gillespie {
	Network *tecido;

public:
	Gillespie(Network *rede) {
		tecido = rede;

		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		cout << "(" << j << "," << i << ") " << tecido->getId(i, j, trunc(DIM_Z / 2)) << " " << tecido->get(i, j, trunc(DIM_Z / 2), "C") << " ";
		// 	}
		// 	cout << endl;
		// }

		//cout << sigma0(0) << endl;
	}

	// Reaction 1
	double sigma0(int id) {
		return tecido->get(id, "vin");
	}

	// Reaction 2
	double sigma1(int id) {
		return 4 * tecido->get(id, "VM3") * ((pow(tecido->get(id, "kcaaa"), tecido->get(id, "n")) * pow(tecido->get(id, "C"), tecido->get(id, "n")) ) / ((pow(tecido->get(id, "C"), tecido->get(id, "n")) + pow(tecido->get(id, "kcaaa"), tecido->get(id, "n"))) * (pow(tecido->get(id, "C"), tecido->get(id, "n")) + pow(tecido->get(id, "kcai"), tecido->get(id, "n"))) ) * (pow(tecido->get(id, "IP3"), tecido->get(id, "m")) / (pow(tecido->get(id, "kip3"), tecido->get(id, "m")) + pow(tecido->get(id, "IP3"), tecido->get(id, "m")) ) )) * (tecido->get(id, "ER") - tecido->get(id, "C"));
	}

	// Reaction 3
	double sigma2(int id) {
		return tecido->get(id, "VM2") * ( pow(tecido->get(id, "C"), 2) / (pow(tecido->get(id, "K2"), 2) + pow(tecido->get(id, "C"), 2)) );
	}

	// Reaction 4
	double kf_Ea(int id) {
		return tecido->get(id, "kf") * tecido->get(id, "ER");
	}

	// Reaction 5
	double kf_Ca(int id) {
		return tecido->get(id, "kf") * tecido->get(id, "C");
	}

	// Reaction 6
	double ko_Ca(int id) {
		return tecido->get(id, "ko") * tecido->get(id, "C");
	}

	// Reaction 7
	double sigma3(int id) {
		return tecido->get(id, "vp") * (pow(tecido->get(id, "C"), 2) / (pow(tecido->get(id, "C"), 2) + pow(tecido->get(id, "kp"), 2)) );
	}

	// Reaction 8
	double kd_Ia(int id) {
		return tecido->get(id, "kdeg") * tecido->get(id, "IP3");
	}

	// Reaction 9
	vector<double> diffusions(int id) {
		int nConnections = tecido->numberConnections();
		vector<double> diffusions(nConnections * 3);

		vector<int> connections(nConnections);
		connections = tecido->getConnections(id);
		double value;

		for (int i = 0; i < connections.size(); i++){
			for (int gj = 0; gj < 3; gj++) {
				if (connections[i] != -1)
					value = diffusionEquation(id, connections[i], gj);
				else
					value = 0;

				diffusions[(3 * i) + gj] = value;
			}
		}

		return diffusions;
	};
	double diffusionEquation(int id1, int id2, int gap_junction) {
		double vol_cell = (4 / 3) * (PI * pow((diameter_cell / 2), 3));
		double diff;

		if (tecido->get(id1, "C") <= tecido->get(id2, "C"))
			return 0;

		if (gap_junction == 0) {
			diff = (tecido->get(id1, "D") / vol_cell) * (tecido->get(id1, "C") - tecido->get(id2, "C")) * tecido->get(id1, "phh");
		} else if (gap_junction == 1) {
			diff = (tecido->get(id1, "D") / vol_cell) * (tecido->get(id1, "C") - tecido->get(id2, "C")) * tecido->get(id1, "phl");
		} else {
			diff = (tecido->get(id1, "D") / vol_cell) * (tecido->get(id1, "C") - tecido->get(id2, "C")) * tecido->get(id1, "plh");
		}

		return diff;
	}

	// Reaction 10
	vector<double> Na_i_diffusions(int id) {
		int nConnections = tecido->numberConnections();
		vector<double> diffusions(nConnections * 3);

		vector<int> connections(nConnections);
		connections = tecido->getConnections(id);
		double value;

		for (int i = 0; i < connections.size(); i++){
			for (int gj = 0; gj < 3; gj++) {
				if (connections[i] != -1)
					value = Na_i_diffusionEquation(id, connections[i], gj);
				else
					value = 0;

				diffusions[(3 * i) + gj] = value;
			}
		}

		return diffusions;
	};
	double Na_i_diffusionEquation(int id1, int id2, int gap_junction) {
		double vol_cell = (4 / 3) * (PI * pow((diameter_cell / 2), 3));
		double diff;

		if (tecido->get(id1, "Na_i") <= tecido->get(id2, "Na_i"))
			return 0;

		if (gap_junction == 0) {
			diff = (tecido->get(id1, "NaD") / vol_cell) * (tecido->get(id1, "Na_i") - tecido->get(id2, "Na_i")) * tecido->get(id1, "phh");
		} else if (gap_junction == 1) {
			diff = (tecido->get(id1, "NaD") / vol_cell) * (tecido->get(id1, "Na_i") - tecido->get(id2, "Na_i")) * tecido->get(id1, "phl");
		} else {
			diff = (tecido->get(id1, "NaD") / vol_cell) * (tecido->get(id1, "Na_i") - tecido->get(id2, "Na_i")) * tecido->get(id1, "plh");
		}

		return diff;
	}

	// Reaction 11
	vector<double> Ca_o_diffusions(int id) {
		int nConnections = tecido->numberConnections();
		vector<double> diffusions(nConnections * 3);

		vector<int> connections(nConnections);
		connections = tecido->getConnections(id);
		double value;

		for (int i = 0; i < connections.size(); i++){
			for (int gj = 0; gj < 3; gj++) {
				if (connections[i] != -1)
					value = Ca_o_diffusionEquation(id, connections[i], gj);
				else
					value = 0;

				diffusions[(3 * i) + gj] = value;
			}
		}

		return diffusions;
	};
	double Ca_o_diffusionEquation(int id1, int id2, int gap_junction) {
		double vol_cell = (4 / 3) * (PI * pow((diameter_cell / 2), 3));
		double diff;

		if (tecido->get(id1, "C_o") <= tecido->get(id2, "C_o"))
			return 0;

		if (gap_junction == 0) {
			diff = (tecido->get(id1, "D") / vol_cell) * (tecido->get(id1, "C_o") - tecido->get(id2, "C_o")) * tecido->get(id1, "phh");
		} else if (gap_junction == 1) {
			diff = (tecido->get(id1, "D") / vol_cell) * (tecido->get(id1, "C_o") - tecido->get(id2, "C_o")) * tecido->get(id1, "phl");
		} else {
			diff = (tecido->get(id1, "D") / vol_cell) * (tecido->get(id1, "C_o") - tecido->get(id2, "C_o")) * tecido->get(id1, "plh");
		}

		return diff;
	}

	// Reaction 12
	vector<double> Na_o_diffusions(int id) {
		int nConnections = tecido->numberConnections();
		vector<double> diffusions(nConnections * 3);

		vector<int> connections(nConnections);
		connections = tecido->getConnections(id);
		double value;

		for (int i = 0; i < connections.size(); i++){
			for (int gj = 0; gj < 3; gj++) {
				if (connections[i] != -1)
					value = Na_o_diffusionEquation(id, connections[i], gj);
				else
					value = 0;

				diffusions[(3 * i) + gj] = value;
			}
		}

		return diffusions;
	};
	double Na_o_diffusionEquation(int id1, int id2, int gap_junction) {
		double vol_cell = (4 / 3) * (PI * pow((diameter_cell / 2), 3));
		double diff;

		if (tecido->get(id1, "Na_o") <= tecido->get(id2, "Na_o"))
			return 0;

		if (gap_junction == 0) {
			diff = (tecido->get(id1, "NaD") / vol_cell) * (tecido->get(id1, "Na_o") - tecido->get(id2, "Na_o")) * tecido->get(id1, "phh");
		} else if (gap_junction == 1) {
			diff = (tecido->get(id1, "NaD") / vol_cell) * (tecido->get(id1, "Na_o") - tecido->get(id2, "Na_o")) * tecido->get(id1, "phl");
		} else {
			diff = (tecido->get(id1, "NaD") / vol_cell) * (tecido->get(id1, "Na_o") - tecido->get(id2, "Na_o")) * tecido->get(id1, "plh");
		}

		return diff;
	}

	//Reaction 13 - NCX
	double Na_Ca_exchanger(int id, int *NCX_mode) {
		double dCa_dt, J_NCX, Allo, Delta_E_num, Delta_E_denom;
		//cout << "Entrei no NCX " << endl;
		//(tecido->get(id, "Erev") >= tecido->get(id, "Vm")) &&
		// Modo reverso: Potencial de reversão >= Potencial de repouso; DDC de Na (intra-extra) >= Concentracao de Ref  
		if (tecido->get(id, "Na_i") >= 15000 && tecido->get(id, "C") <= 0.5) { // Kirischuk 2012 [uM]
			// Eq. (6) from Koenigsberger, 2004
			*NCX_mode = 1;
			J_NCX = tecido->get(id, "G_NCX")*tecido->get(id, "C")*(tecido->get(id, "Vm") - tecido->get(id, "V_NCX"))/(tecido->get(id, "C") + tecido->get(id, "C_NCX"));
			/*
			Allo = 1/(1 + pow(tecido->get(id, "K_mCaAct")/tecido->get(id, "C"), tecido->get(id, "n_Hill")));
			Delta_E_num = tecido->get(id, "J_max")*( pow(tecido->get(id, "Na_i"),3)*tecido->get(id, "C_o")*exp(tecido->get(id, "eta")*(tecido->get(id, "V_NCX")/1000)*F/(R*T)) - pow(tecido->get(id, "Na_o"),3)*tecido->get(id, "C")*exp((tecido->get(id, "eta")-1)*(tecido->get(id, "V_NCX")/1000)*F/(R*T)));
			Delta_E_denom = ( tecido->get(id, "K_mCao")*pow(tecido->get(id, "Na_i"),3) + pow(tecido->get(id, "K_mNao"),3)*tecido->get(id, "C") + pow(tecido->get(id, "K_mNai"),3)*tecido->get(id, "C_o")*(1+tecido->get(id, "C")/tecido->get(id, "K_mCai")) + tecido->get(id, "K_mCai")*pow(tecido->get(id, "Na_o"),3)*(1+pow(tecido->get(id, "Na_i")/tecido->get(id, "K_mCai"),3)) + pow(tecido->get(id, "Na_i"),3)*tecido->get(id, "C_o") + pow(tecido->get(id, "Na_o"),3)*tecido->get(id, "C") )*(1 + tecido->get(id, "k_sat")*exp((tecido->get(id, "eta")-1)*(tecido->get(id, "V_NCX")/1000)*F/(R*T)));
			
			J_NCX = Allo*Delta_E_num/Delta_E_denom;
			//dCa_dt = J_NCX - (tecido->get(id, "C_rest")-tecido->get(id, "C"))/tecido->get(id, "tau_Ca");

			*/
			// cout << setprecision(16) << "Reverse Mode -- J_NCX = " << J_NCX << endl;
			//tecido->get(id, "vin");
			//cout << "Reverse Mode = " << tecido->get(id, "Vm") << endl;
			return J_NCX;

		}
		//(tecido->get(id, "Erev") < tecido->get(id, "Vm")) && 
		// Modo direto: Potencial de reversão < Potencial de repouso; DDC de Na (intra-extra) < Concentracao de Ref
		else if (tecido->get(id, "Na_i") < 15000 && tecido->get(id, "C") > 0.5) { // Kirischuk 2012 [uM]
			// Eq. (6) from Koenigsberger, 2004
			*NCX_mode = 2;
			J_NCX = tecido->get(id, "G_NCX")*tecido->get(id, "C")*(tecido->get(id, "Vm") - tecido->get(id, "V_NCX"))/(tecido->get(id, "C") + tecido->get(id, "C_NCX"));
			/*
			Allo = 1/(1 + pow(tecido->get(id, "K_mCaAct")/tecido->get(id, "C"), tecido->get(id, "n_Hill")));
			Delta_E_num = tecido->get(id, "J_max")*( pow(tecido->get(id, "Na_i"),3)*tecido->get(id, "C_o")*exp(tecido->get(id, "eta")*(tecido->get(id, "V_NCX")/1000)*F/(R*T)) - pow(tecido->get(id, "Na_o"),3)*tecido->get(id, "C")*exp((tecido->get(id, "eta")-1)*(tecido->get(id, "V_NCX")/1000)*F/(R*T)));
			Delta_E_denom = ( tecido->get(id, "K_mCao")*pow(tecido->get(id, "Na_i"),3) + pow(tecido->get(id, "K_mNao"),3)*tecido->get(id, "C") + pow(tecido->get(id, "K_mNai"),3)*tecido->get(id, "C_o")*(1+tecido->get(id, "C")/tecido->get(id, "K_mCai")) + tecido->get(id, "K_mCai")*pow(tecido->get(id, "Na_o"),3)*(1+pow(tecido->get(id, "Na_i")/tecido->get(id, "K_mCai"),3)) + pow(tecido->get(id, "Na_i"),3)*tecido->get(id, "C_o") + pow(tecido->get(id, "Na_o"),3)*tecido->get(id, "C") )*(1 + tecido->get(id, "k_sat")*exp((tecido->get(id, "eta")-1)*(tecido->get(id, "V_NCX")/1000)*F/(R*T)));
			
			J_NCX = Allo*Delta_E_num/Delta_E_denom;
			//dCa_dt = J_NCX + (tecido->get(id, "C_rest")-tecido->get(id, "C"))/tecido->get(id, "tau_Ca");

			*///cout << setprecision(16) << "Forward Mode -- J_NCX = " << J_NCX << endl;
			//tecido->get(id, "ko") * tecido->get(id, "C");
			//cout << "Forward Mode = " << tecido->get(id, "Vm") << endl;
			return J_NCX;
		}
		else{
			*NCX_mode = 0;
			return 0;
		}

	}

	//Reaction 14 - Na+/K+ Pump
	double Na_K_pump(int id) {
		
		if (tecido->get(id,"ATP") > ATP_THRESHOLD) 
		{
			// double I_p = e_0 * (tecido->get(id, "J_Np") + tecido->get(id, "J_Kp"));
			double V = (4 / 3) * (PI * pow((diameter_cell / 2), 3));

			//double J_Nl = tecido->get(id, "P_N") * tecido->get(id, "A_vesicles") * (tecido->get(id, "Vm") / (R * T / F)) * (tecido->get(id, "c_N_i")  * exp(tecido->get(id, "Vm") / (R * T / F)) - tecido->get(id, "c_N_o")) / (exp(tecido->get(id, "Vm") / (R * T / F)) - 1);
			//double J_Kl = tecido->get(id, "P_K") * tecido->get(id, "A_vesicles") * (tecido->get(id, "Vm") / (R * T / F)) * (tecido->get(id, "c_K_i") * exp(tecido->get(id, "Vm") / (R * T / F)) - tecido->get(id, "c_K_o")) / (exp(tecido->get(id, "Vm") / (R * T / F)) - 1);

			double J_Nl = tecido->get(id, "P_N") * tecido->get(id, "A_vesicles") * (tecido->get(id, "Vm")) * (tecido->get(id, "c_N_i")  * exp(tecido->get(id, "Vm")) - tecido->get(id, "c_N_o")) / (exp(tecido->get(id, "Vm")) - 1);
			double J_Kl = tecido->get(id, "P_K") * tecido->get(id, "A_vesicles") * (tecido->get(id, "Vm")) * (tecido->get(id, "c_K_i") * exp(tecido->get(id, "Vm")) - tecido->get(id, "c_K_o")) / (exp(tecido->get(id, "Vm")) - 1);
			
			double J_N = (tecido->get(id, "J_Np") + J_Nl) / V;
			double J_K = (tecido->get(id, "J_Kp") + J_Kl) / V;

			// cout << "Exp: " << exp(tecido->get(id, "Vm")) << endl;
			// cout << "J_K: " << J_K << endl;

			// double I = I_p + e_0 * (J_Nl + J_Kl) + lambda * tecido->get(id, "A_vesicles") * tecido->get(id, "Vm");

			return J_K;
		}

		return 0;
	}

	//Reaction 15 - Ca+ Pump
	double Ca_pump(int id) {
		
		if (tecido->get(id,"ATP") > ATP_THRESHOLD) { // uM - ENCONTRAR THRESHOLD !!!!!!!!!!!!

			double J_cp = tecido->get(id, "G_cp") * pow(tecido->get(id, "C"), 2) / (pow(tecido->get(id, "C"), 2) + tecido->get(id, "K_cp"));

			return J_cp;
		}

		return 0;
	}

	// REACTIONS - FIM

	vector<double> calciumReactions() {
		int nConnections = tecido->numberConnections();
		int NC = DIM_X * DIM_Y * DIM_Z; // Total number of cells
		int num_reactions = 9; // Number of reactions (7 Intracellular + 1 Intercelular)
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		vector<double> diffusion(nConnections * 3);
		double reactions[DIM_Y][DIM_X][DIM_Z][num_reactions + nConnections * 3 - 1];

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {

				for (int k = 0; k < DIM_Z; k++) {
					// << Begin Reactions
					for (int r = 0; r < num_reactions; r++) {
						
						tecido->update_parameters(i, j , k, "Vm");

						if (r == 0) {
							reaction_value = sigma0(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 1) {
							reaction_value = sigma1(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 2) {
							reaction_value = sigma2(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 3) {
							reaction_value = kf_Ea(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 4) {
							reaction_value = kf_Ca(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 5) {
							reaction_value = ko_Ca(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 6) {
							reaction_value = sigma3(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 7) {
							reaction_value = kd_Ia(tecido->getId(i, j, k));
							reactions[j][i][k][r] = reaction_value;
						} else if (r == 8) {
							diffusion = diffusions(tecido->getId(i, j, k));
							for (int d = 0; d < diffusion.size(); d++) {
								reaction_value = diffusion[d];
								reactions[j][i][k][r + d] = reaction_value;

								/*if (reaction_value >= max_reaction) {
									max_reaction = reaction_value;
									cell[0] = j; cell[1] = i; cell[2] = k;
									if (d >= 0 && d <= 2)
										reaction_choice = 8;
									else if (d >= 3 && d <= 5)
										reaction_choice = 9;
									else if (d >= 6 && d <= 8)
										reaction_choice = 10;
									else if (d >= 9 && d <= 11)
										reaction_choice = 11;
									else if (d >= 12 && d <= 14)
										reaction_choice = 12;
									else
										reaction_choice = 13;
								}*/

								alfa_0 += reaction_value;
							}
						}
						
						if (r != 8) {
							/*if (reaction_value >= max_reaction) {
								max_reaction = reaction_value;
								cell[0] = j; cell[1] = i; cell[2] = k;
								reaction_choice = r;
							}*/
							
							alfa_0 += reaction_value;
						}
						//cout << reaction_value << endl;
					}
					// End Reactions >>
				}
			}
		}


		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 			for (int r = 0; r < num_reactions; r++) {
		// 				cout << reactions[i][j][k][r] << " ";
		// 			}
		// 			cout << endl;
		// 		}
		// 	}
		// }


		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		//cout << setprecision(8) << "alfa0 " << alfa_0 << endl;
		double tau = (1 / alfa_0) * log(1 / r1);

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r < (num_reactions + nConnections * 3 - 1); r++) {
						sum_upper += reactions[j][i][k][r];

						if (sum_upper >= alfa_0 * r2) {
							//cout << i << " " << j << " " << k << " " << r << endl;

							flag = false;
							sum_down = 0;
							for (int x = 0; x < DIM_X && flag == false; x++) {
								for (int y = 0; y < DIM_Y && flag == false; y++) {
									for (int z = 0; z < DIM_Z && flag == false; z++) {
										for (int n = 0; n < (num_reactions + nConnections * 3 - 1) && flag == false; n++) {
											if (x == i && y == j && z == k && n == r) {
												//cout << x << " " << y << " " << z << " " << n << endl;
												//cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
												flag = true;
											} else {
												sum_down += reactions[y][x][z][n];
											}
										}
									}
								}
							}

							if (sum_down < alfa_0 * r2) {
								//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
								retorno[0] = r + 1; // [1, 26]
								retorno[1] = i;
								retorno[2] = j;
								retorno[3] = k;
								retorno[4] = tau;
								//cout << setprecision(5) << tau << endl;

								return retorno;
							}
						}
					}
				}
			}
		}
	}

	vector<double> sodiumInter() {
		int nConnections = tecido->numberConnections();
		int NC = DIM_X * DIM_Y * DIM_Z; // Total number of cells
		int num_reactions = 1; 
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		vector<double> Na_i_diffusion(nConnections * 3);
		double reactions[DIM_Y][DIM_X][DIM_Z][num_reactions + (nConnections * 3 - 1)]; //0 a 43

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {

				for (int k = 0; k < DIM_Z; k++) {
					// << Begin Reactions
					Na_i_diffusion = Na_i_diffusions(tecido->getId(i, j, k));
					for (int d = 0; d < Na_i_diffusion.size(); d++) {
						reaction_value = Na_i_diffusion[d];
						reactions[j][i][k][d] = reaction_value;
						//printf("alfa_sodium = %f \n", reaction_value);
						alfa_0 += reaction_value;
					}
					// End Reactions >>
					
				}
			}
		}


		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 			for (int r = 0; r < num_reactions; r++) {
		// 				cout << reactions[i][j][k][r] << " ";
		// 			}
		// 			cout << endl;
		// 		}
		// 	}
		// }


		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		double tau = (1 / alfa_0) * log(1 / r1);

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r < (num_reactions + (nConnections * 3 - 1)); r++) {
						// cout << "R: " << r <<endl;
						sum_upper += reactions[j][i][k][r];

						if (sum_upper >= alfa_0 * r2) {
							//cout << i << " " << j << " " << k << " " << r << endl;

							flag = false;
							sum_down = 0;
							for (int x = 0; x < DIM_X && flag == false; x++) {
								for (int y = 0; y < DIM_Y && flag == false; y++) {
									for (int z = 0; z < DIM_Z && flag == false; z++) {
										for (int n = 0; n < (num_reactions +  (nConnections * 3 - 1)) && flag == false; n++) { //0 a 43 -> tem que ir
											// cout << "N: " << r <<endl;

											if (x == i && y == j && z == k && n == r) {
												// cout << x << " " << y << " " << z << " " << n << endl;
												// cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
												flag = true;
											} else {
												sum_down += reactions[y][x][z][n];
											}
										}
									}
								}
							}

							if (sum_down < alfa_0 * r2) {
								//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
								retorno[0] = r + 1; // [1, 26]
								retorno[1] = i;
								retorno[2] = j;
								retorno[3] = k;
								retorno[4] = tau;
								// cout << "Reaction: " << r+1 <<endl;
								// cout << fixed << setprecision(10) << "Tau: " << tau <<endl;
								return retorno;
							}
						}
					}
				}
			}
		}
	}

	vector<double> sodiumExtra() {
		int nConnections = tecido->numberConnections();
		int NC = DIM_X * DIM_Y * DIM_Z; // Total number of cells
		int num_reactions = 1; 
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		vector<double> Na_o_diffusion(nConnections * 3);
		double reactions[DIM_Y][DIM_X][DIM_Z][num_reactions + (nConnections * 3 - 1)]; //0 a 43

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {

				for (int k = 0; k < DIM_Z; k++) {
					// << Begin Reactions
					Na_o_diffusion = Na_o_diffusions(tecido->getId(i, j, k));
					for (int d = 0; d < Na_o_diffusion.size(); d++) {
						reaction_value = Na_o_diffusion[d];
						reactions[j][i][k][d] = reaction_value;

						alfa_0 += reaction_value;
					}
					// End Reactions >>

				}
			}
		}


		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 			for (int r = 0; r < num_reactions; r++) {
		// 				cout << reactions[i][j][k][r] << " ";
		// 			}
		// 			cout << endl;
		// 		}
		// 	}
		// }


		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		double tau = (1 / alfa_0) * log(1 / r1);

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r < (num_reactions + (nConnections * 3 - 1)); r++) {
						// cout << "R: " << r <<endl;
						sum_upper += reactions[j][i][k][r];

						if (sum_upper >= alfa_0 * r2) {
							//cout << i << " " << j << " " << k << " " << r << endl;

							flag = false;
							sum_down = 0;
							for (int x = 0; x < DIM_X && flag == false; x++) {
								for (int y = 0; y < DIM_Y && flag == false; y++) {
									for (int z = 0; z < DIM_Z && flag == false; z++) {
										for (int n = 0; n < (num_reactions +  (nConnections * 3 - 1)) && flag == false; n++) { //0 a 43 -> tem que ir
											// cout << "N: " << r <<endl;

											if (x == i && y == j && z == k && n == r) {
												// cout << x << " " << y << " " << z << " " << n << endl;
												// cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
												flag = true;
											} else {
												sum_down += reactions[y][x][z][n];
											}
										}
									}
								}
							}

							if (sum_down < alfa_0 * r2) {
								//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
								retorno[0] = r + 1; // [1, 26]
								retorno[1] = i;
								retorno[2] = j;
								retorno[3] = k;
								retorno[4] = tau;
								// cout << "Reaction: " << r+1 <<endl;
								// cout << fixed << setprecision(10) << "Tau: " << tau <<endl;
								return retorno;
							}
						}
					}
				}
			}
		}
	}

	vector<double> calciumExtra() {
		int nConnections = tecido->numberConnections();
		int NC = DIM_X * DIM_Y * DIM_Z; // Total number of cells
		int num_reactions = 1; 
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		vector<double> Ca_o_diffusion(nConnections * 3);
		double reactions[DIM_Y][DIM_X][DIM_Z][num_reactions + (nConnections * 3 - 1)]; //0 a 43

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					// << Begin Reactions

					Ca_o_diffusion = Ca_o_diffusions(tecido->getId(i, j, k));
					for (int d = 0; d < Ca_o_diffusion.size(); d++) {
						reaction_value = Ca_o_diffusion[d];
						reactions[j][i][k][d] = reaction_value;

						alfa_0 += reaction_value;
					}
					// End Reactions >>

				}
			}
		}


		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 			for (int r = 0; r < num_reactions; r++) {
		// 				cout << reactions[i][j][k][r] << " ";
		// 			}
		// 			cout << endl;
		// 		}
		// 	}
		// }


		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		double tau = (1 / alfa_0) * log(1 / r1);

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r < (num_reactions + (nConnections * 3 - 1)); r++) {
						// cout << "R: " << r <<endl;
						sum_upper += reactions[j][i][k][r];

						if (sum_upper >= alfa_0 * r2) {
							//cout << i << " " << j << " " << k << " " << r << endl;

							flag = false;
							sum_down = 0;
							for (int x = 0; x < DIM_X && flag == false; x++) {
								for (int y = 0; y < DIM_Y && flag == false; y++) {
									for (int z = 0; z < DIM_Z && flag == false; z++) {
										for (int n = 0; n < (num_reactions +  (nConnections * 3 - 1)) && flag == false; n++) { //0 a 43 -> tem que ir
											// cout << "N: " << r <<endl;

											if (x == i && y == j && z == k && n == r) {
												// cout << x << " " << y << " " << z << " " << n << endl;
												// cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
												flag = true;
											} else {
												sum_down += reactions[y][x][z][n];
											}
										}
									}
								}
							}

							if (sum_down < alfa_0 * r2) {
								//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
								retorno[0] = r + 1; // [1, 26]
								retorno[1] = i;
								retorno[2] = j;
								retorno[3] = k;
								retorno[4] = tau;
								// cout << "Reaction: " << r+1 <<endl;
								// cout << fixed << setprecision(10) << "Tau: " << tau <<endl;
								return retorno;
							}
						}
					}
				}
			}
		}
	}

	vector<double> NCX_reaction(int* NCX_mode_vector) {
		int NCX_mode, num_reactions = 20; // number of reactions
		int NCX_mode_array[DIM_Y][DIM_X][DIM_Z][num_reactions];
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		double reactions[DIM_Y][DIM_X][DIM_Z];

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r < num_reactions; r++) {
						tecido->update_parameters(i, j , k, "Vm");
						
						reaction_value = Na_Ca_exchanger(tecido->getId(i, j, k), &NCX_mode);
						reactions[j][i][k] += reaction_value;
						NCX_mode_array[j][i][k][r] = NCX_mode;
						
						alfa_0 += reaction_value;
						// cout << reaction_value << endl;
					}
				}
			}
		}

		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 				cout << reactions[i][j][k][r] << " ";
		// 			cout << endl;
		// 		}
		// 	}
		// }

		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		//cout << setprecision(8) << "alfa0 " << alfa_0 << endl;
		double tau = (1 / alfa_0) * log(1 / r1);

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					sum_upper += reactions[j][i][k];

					if (sum_upper >= alfa_0 * r2) {
						//cout << i << " " << j << " " << k << " " << r << endl;

						flag = false;
						sum_down = 0;
						for (int x = 0; x < DIM_X && flag == false; x++) {
							for (int y = 0; y < DIM_Y && flag == false; y++) {
								for (int z = 0; z < DIM_Z && flag == false; z++) {
									if (x == i && y == j && z == k) {
										//cout << x << " " << y << " " << z << " " << n << endl;
										//cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
										flag = true;
									} else {
										sum_down += reactions[y][x][z];
									}
								}
							}
						}
						if (sum_down < alfa_0 * r2) {
							//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
							for (int n=0; n<num_reactions; n++){
								NCX_mode_vector[n] = NCX_mode_array[j][i][k][n];
							}
							retorno[0] = 0;
							retorno[1] = i;
							retorno[2] = j;
							retorno[3] = k;
							retorno[4] = tau;
							//cout << tau << endl;

							return retorno;
						}
					}
				}
			}
		}
	}

	vector<double> NKATP_reaction() {
		int NCX_mode, num_reactions = 0; // number of reactions
		int NCX_mode_array[DIM_Y][DIM_X][DIM_Z][num_reactions];
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		double reactions[DIM_Y][DIM_X][DIM_Z];

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r <= num_reactions; r++) {
						
						reaction_value = Na_K_pump(tecido->getId(i, j, k));
						reactions[j][i][k] += reaction_value;
						
						alfa_0 += reaction_value;
						//cout << "NKP Reaction value: " << reaction_value << endl;
					}
				}
			}
		}

		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 				cout << reactions[i][j][k][r] << " ";
		// 			cout << endl;
		// 		}
		// 	}
		// }

		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		//cout << setprecision(8) << "alfa0 " << alfa_0 << endl;
		double tau = (1 / alfa_0) * log(1 / r1);
		// cout << setprecision(8) << "Tau NKATP: " << tau << endl;

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					sum_upper += reactions[j][i][k];

					if (sum_upper >= alfa_0 * r2) {
						//cout << i << " " << j << " " << k << " " << r << endl;

						flag = false;
						sum_down = 0;
						for (int x = 0; x < DIM_X && flag == false; x++) {
							for (int y = 0; y < DIM_Y && flag == false; y++) {
								for (int z = 0; z < DIM_Z && flag == false; z++) {
									if (x == i && y == j && z == k) {
										//cout << x << " " << y << " " << z << " " << n << endl;
										//cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
										flag = true;
									} else {
										sum_down += reactions[y][x][z];
									}
								}
							}
						}
						if (sum_down < alfa_0 * r2) {
							//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
							
							retorno[0] = 0;
							retorno[1] = i;
							retorno[2] = j;
							retorno[3] = k;
							retorno[4] = tau;
							// cout << "Tempo: " << tau << endl;

							return retorno;
						}
					}
				}
			}
		}
	}

	vector<double> CP_reaction() {
		int NCX_mode, num_reactions = 0; // number of reactions
		int NCX_mode_array[DIM_Y][DIM_X][DIM_Z][num_reactions];
		double max_reaction = 0, reaction_choice, alfa_0 = 0, reaction_value;
		vector<double> retorno(5);
		double reactions[DIM_Y][DIM_X][DIM_Z];

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					for (int r = 0; r <= num_reactions; r++) {
						
						reaction_value = Ca_pump(tecido->getId(i, j, k));
						reactions[j][i][k] += reaction_value;
						
						alfa_0 += reaction_value;
						//cout << "CP Reaction Value: " << reaction_value << endl;
					}
				}
			}
		}

		// for (int i = 0; i < DIM_X; i++) {
		// 	for (int j = 0; j < DIM_Y; j++) {
		// 		for (int k = 0; k < DIM_Z; k++) {
		// 			cout << tecido->getId(i, j, k) << ": ";
		// 				cout << reactions[i][j][k][r] << " ";
		// 			cout << endl;
		// 		}
		// 	}
		// }

		// Gerando dois números aleatórios: r1 e r2
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator (seed);
		std::uniform_real_distribution<double> distribution (0.0,1.0);

		double r1 = distribution(generator);
		double r2 = distribution(generator);

		// Calculando o tempo tau
		//cout << setprecision(8) << "alfa0 " << alfa_0 << endl;
		double tau = (1 / alfa_0) * log(1 / r1);
		// cout << setprecision(8) << "Tau CP: " << tau << endl;

		// Definindo a reação que será executada
		double sum_upper = 0, sum_down = 0;
		bool flag = false;

		for (int i = 0; i < DIM_X; i++) {
			for (int j = 0; j < DIM_Y; j++) {
				for (int k = 0; k < DIM_Z; k++) {
					sum_upper += reactions[j][i][k];

					if (sum_upper >= alfa_0 * r2) {
						//cout << i << " " << j << " " << k << " " << r << endl;

						flag = false;
						sum_down = 0;
						for (int x = 0; x < DIM_X && flag == false; x++) {
							for (int y = 0; y < DIM_Y && flag == false; y++) {
								for (int z = 0; z < DIM_Z && flag == false; z++) {
									if (x == i && y == j && z == k) {
										//cout << x << " " << y << " " << z << " " << n << endl;
										//cout << sum_down << " " << alfa_0 * r2 << " " << sum_upper << endl;
										flag = true;
									} else {
										sum_down += reactions[y][x][z];
									}
								}
							}
						}
						if (sum_down < alfa_0 * r2) {
							//cout << sum_upper << " " << alfa_0 * r2 << " " << sum_down << endl;
							
							retorno[0] = 0;
							retorno[1] = i;
							retorno[2] = j;
							retorno[3] = k;
							retorno[4] = tau;
							// cout << "Tempo: " << tau << endl;

							return retorno;
						}
					}
				}
			}
		}
	}

};

void simulation(int destination, double frequency, string topology, double time_slot, ofstream& file_results, string fileName) { 
	Network tecido;
	int tx_x = trunc(DIM_X / 2);
	int tx_y = trunc(DIM_Y / 2);
	int tx_z = trunc(DIM_Z / 2);
	int radius = 0;

	// DEFININDO A TOPOLOGIA DO TECIDO
	if (topology == "RD"){
		tecido.regularDegree();
		radius = 1;
	}
	else if (topology == "LR2"){ 
		tecido.linkRadius(2);
		radius = 2;
	}
	else if (topology == "LR3"){
		tecido.linkRadius(3);
		radius = 3;
	}

	// SETTING THE VALUES
	tecido.set(tx_x, tx_y, tx_z, "C", 0.5*SCALE); // Quais as referências?
	// tecido.set(tx_x, tx_y, tx_z, "Na_i", 20000*SCALE); // Quais as referências?
	// tecido.set(tx_x, tx_y, tx_z, "C_o", 20*SCALE); // Quais as referências?
	// tecido.set(tx_x, tx_y, tx_z, "Na_o", 1000*SCALE); // Quais as referências?

	// Print tissue
	// tecido.printTissue();

	// Opening file containing the data of calcium concentration that will be plotted
	// ofstream exportfile;
	// exportfile.open("temp/data.txt");
	// tecido.writeFileHeader(exportfile);
	// tecido.printTissuef(exportfile, 0); // Writes the tissue's initial state to the file
	
	// Inicializando o Algoritmo de Gillespie
	Gillespie gillespie(&tecido);

	int nConnections = tecido.numberConnections(), num_reactions = 20;
	int reaction, int_time = 0, x_c, y_c, z_c;
	int total_reactions = 0, greaterThanTimeScounter = 0;
	int NCX_mode_vector[num_reactions] = {0};

	// cout << "Connections per cell: " << nConnections << endl;
	
	vector<double> choice(5);
	vector<double> C_tx, C_rx;
	vector<int> connections(nConnections), qtd_reactions(9 + QTD_DIFFUSIONS * (nConnections * 3)), Tx_states;
	vector<int>::iterator first;

	double simulation_time = 200, current_time = 0, current_time_calcium = 0, current_time_sodium_inter = 0, current_time_sodium_extra = 0, current_time_calcium_extra = 0, current_time_NCX = 0, current_time_NCX_AUX = 0, current_time_calcium_aux = 0;
	double tau_max = 100000, tau_calcium=0, tau_sodium_inter=0, tau_NCX=0, tau_sodium_extra=0, tau_calcium_extra=0, E_signal = 0, E_noise = 0, c_in=0, c_out=0, current_time_modulation = 0;
	double tau_NKATP = 0, tau_CP = 0, current_time_NKATP = 0, current_time_NKATP_AUX = 0, current_time_CP = 0, current_time_CP_aux = 0; 

	bool diffusion_error = false, tau_flag = false;

	while (simulation_time > current_time) 
	{
		tau_flag 				   = false;
		current_time_calcium 	   = 0; 
		current_time_sodium_inter  = 0; 
		current_time_sodium_extra  = 0; 
		current_time_calcium_extra = 0; 
		current_time_NCX 		   = 0;
		current_time_NKATP 		   = 0;
		current_time_CP 		   = 0;
		greaterThanTimeScounter    = 0;
		tau_max 				   = 100000;
		
		do
		{
			// Updating time and concentrations
			if (trunc(current_time) != int_time) {
				int_time = trunc(current_time);

				// cout << "Time: " << int_time << endl;

				// Print Tissue
				// tecido.printTissue();
				// cout << endl;	
				// End Print Tissue

				// Write Calcium concentration of tissue on file
				// tecido.printTissuef(exportfile, int_time);
				// End Print Tissue

				// Update Gap Junctions
				if (int_time < 50) {
					for (int i = 0; i < DIM_X; i++) {
						for (int j = 0; j < DIM_Y; j++) {
							for (int k = 0; k < DIM_Z; k++) {
								tecido.set(i, j, k, "phl", phl[int_time]);
								tecido.set(i, j, k, "plh", plh[int_time]);
								tecido.set(i, j, k, "phh", phh[int_time]);
							}
						}
					}
				}

				// Calcium and sodium oscillations
				if (int_time % ((int) (1 / frequency)) == 0){
					tecido.accumulate(tx_x, tx_y, tx_z, "C", c_pulse*SCALE);
					//tecido.accumulate(tx_x, tx_y, tx_z, "Na_i", 20000*SCALE);
				}

			}

			// CHOICE OF THE REACTIONS
			diffusion_error = false;

			/* INTRACELULAR CALCIUM REACTIONS */
			if (current_time_calcium<tau_max) 
			{	
				choice 		= gillespie.calciumReactions();
				reaction 	= choice[0];
				tau_calcium = choice[4];
				
				x_c = choice[1];
				y_c = choice[2];
				z_c = choice[3];

				// cout << setprecision(5) << "Tau_calcium = " << tau_calcium << endl;

				qtd_reactions[reaction - 1]++;
				total_reactions++;

				if (reaction == 1) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "C", ALPHA);

					E_noise = E_noise + pow(ALPHA,2);
				} 
				else if (reaction == 2) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "C", ALPHA);
					tecido.accumulate(choice[1], choice[2], choice[3], "ER", -ALPHA);

					if (tecido.get(choice[1], choice[2], choice[3], "ER") < 0) 
					{
						tecido.changeSignal(choice[1], choice[2], choice[3], "ER");
					}
				} 
				else if (reaction == 3) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "C", -ALPHA);
					tecido.accumulate(choice[1], choice[2], choice[3], "ER", ALPHA);

					if (tecido.get(choice[1], choice[2], choice[3], "C") < 0) 
					{
						tecido.changeSignal(choice[1], choice[2], choice[3], "C");
					}
				} 
				else if (reaction == 4) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "C", ALPHA);
					tecido.accumulate(choice[1], choice[2], choice[3], "ER", -ALPHA);

					if (tecido.get(choice[1], choice[2], choice[3], "ER") < 0) 
					{
						tecido.changeSignal(choice[1], choice[2], choice[3], "ER");
					}
				} 
				else if (reaction == 5) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "C", -ALPHA);
					tecido.accumulate(choice[1], choice[2], choice[3], "ER", ALPHA);

					if (tecido.get(choice[1], choice[2], choice[3], "C") < 0) 
					{
						tecido.changeSignal(choice[1], choice[2], choice[3], "C");
					}
				} 
				else if (reaction == 6) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "C", -ALPHA);

					if (tecido.get(choice[1], choice[2], choice[3], "C") < 0) 
					{
						tecido.changeSignal(choice[1], choice[2], choice[3], "C");
					}
					E_noise = E_noise + pow(ALPHA,2);
				} 
				else if (reaction == 7) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "IP3", ALPHA);
				} 
				else if (reaction == 8) 
				{
					tecido.accumulate(choice[1], choice[2], choice[3], "IP3", -ALPHA);

					if (tecido.get(choice[1], choice[2], choice[3], "IP3") < 0) 
					{
						tecido.changeSignal(choice[1], choice[2], choice[3], "IP3");
					}
				}

				/* DIFFUSION REACTIONS OF CALCIUM */
				else 
				{
					for (int conn = 0; conn < nConnections; conn++) 
					{
						if (reaction >= 9 + (conn * 3) && reaction <= 11 + (conn * 3)) 
						{
							connections = tecido.getConnections(tecido.getId(choice[1], choice[2], choice[3]));

							if (connections[conn] != -1 && tecido.get(choice[1], choice[2], choice[3], "C") > tecido.get(connections[conn], "C")) 
							{
								tecido.accumulate(choice[1], choice[2], choice[3], "C", -ALPHA);
								tecido.accumulate(connections[conn], "C", ALPHA);

								// Modulation and Demodulation - Begin
								if ( (x_c == tx_x && y_c == tx_y && z_c == tx_z) )
								{
									tecido.accumulate(x_c, y_c, z_c, "C_variation", ALPHA);
									tecido.accumulate(x_c, y_c, z_c, "tx_rx_reactions", 1);
								}
								
								if ( connections[conn] == tecido.getId(tx_x + destination, tx_y, tx_z) )
								{
									tecido.accumulate( tecido.getId(tx_x + destination, tx_y, tx_z) , "C_variation", ALPHA);
									tecido.accumulate( tecido.getId(tx_x + destination, tx_y, tx_z) , "tx_rx_reactions", 1);
								}
								// Modulation and Demodulation - End
							} 
							else 
							{
								//cout << tecido.getId(choice[1], choice[2], choice[3]) << " " << connections[conn] << endl;
								diffusion_error = true;
							}

							E_signal = E_signal + pow(ALPHA,2);
						}
					}
				}

				current_time_calcium     += tau_calcium;
				current_time_calcium_aux += tau_calcium;
				current_time_modulation  += tau_calcium;

				/* DIFFUSION ERROR => NO INCREMENT TIME */
				if (diffusion_error)
				{
					current_time_calcium 	 -= (tau_calcium);
					current_time_calcium_aux -= (tau_calcium);
					current_time_modulation  -= (tau_calcium);

					diffusion_error 		  = false;
				}
			}
		
			/* INTERCELLULAR SODIUM REACTIONS */
			if (current_time_sodium_inter<tau_max) 
			{
			 	choice = gillespie.sodiumInter();
			 	tau_sodium_inter = choice[4];
			 	reaction = choice[0];
				// cout << "Sodium reactions" << endl;
			 	//cout << setprecision(5) << "Tau_sodium_inter = " << tau_sodium_inter << endl;
			 	for (int conn = 0; conn < nConnections; conn++) 
				{
			 		if (reaction >= 1 + (conn * 3) && reaction <= 3 + (conn * 3)) 
					 {
			 			connections = tecido.getConnections(tecido.getId(choice[1], choice[2], choice[3]));

			 			if (connections[conn] != -1 && tecido.get(choice[1], choice[2], choice[3], "Na_i") > tecido.get(connections[conn], "Na_i")) {
			 				tecido.accumulate(choice[1], choice[2], choice[3], "Na_i", -ALPHA);
			 				tecido.accumulate(connections[conn], "Na_i", ALPHA);
			 			} 
						else 
						{
			 				//cout << tecido.getId(choice[1], choice[2], choice[3]) << " " << connections[conn] << endl;
			 				diffusion_error = true;
			 			}
			 		}
			 	}
			 	current_time_sodium_inter += tau_sodium_inter;
			 	/* DIFFUSION ERROR => NO INCREMENT TIME */
			 	if (diffusion_error) current_time_sodium_inter -= (tau_sodium_inter);
			}

			/* NCX - Reaction */
			if (current_time_NCX < tau_max) 
			{
				choice = gillespie.NCX_reaction(NCX_mode_vector);
				tau_NCX = choice[4];

				for (int i = 0; i < num_reactions; i++)
				{
					if (NCX_mode_vector[i] == 1) // Modo Reverso
					{
						// if (!i) cout << "MODO REVERSO" << endl;
						
						tecido.accumulate(choice[1], choice[2], choice[3], "Na_o", 3*ALPHA);
						tecido.accumulate(choice[1], choice[2], choice[3], "Na_i", -3*ALPHA);
						tecido.accumulate(choice[1], choice[2], choice[3], "C", ALPHA);
						tecido.accumulate(choice[1], choice[2], choice[3], "C_o", -ALPHA);
					} 
					else if (NCX_mode_vector[i] == 2) // Modo Direto
					{
						// if (!i) cout << "MODO DIRETO" << endl;
						
						tecido.accumulate(choice[1], choice[2], choice[3], "Na_i", 3*ALPHA);
						tecido.accumulate(choice[1], choice[2], choice[3], "Na_o", -3*ALPHA);
						tecido.accumulate(choice[1], choice[2], choice[3], "C_o", ALPHA);
						tecido.accumulate(choice[1], choice[2], choice[3], "C", -ALPHA);
					}
					E_noise = E_noise + pow(ALPHA,2);
				}
				current_time_NCX += tau_NCX;
				current_time_NCX_AUX += tau_NCX;
			}

			/* NKATP Reaction */
			// if (current_time_NKATP < tau_max) 
			// {
			// 	choice = gillespie.NKATP_reaction();
			// 	tau_NKATP = choice[4];

			// 	// cout << "NKP functioning..." << endl;
			// 	for (int i = 0; i < 0*num_reactions; i++)
			// 	{
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "Na_i", -3 * ALPHA);
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "Na_o", 3 * ALPHA);
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "K_o", -2 * ALPHA);
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "K_i", 2 * ALPHA);

			// 		tecido.accumulate(choice[1], choice[2], choice[3], "ATP", -ALPHA);
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "ADP", ALPHA);
			// 	}			
			// 	current_time_NKATP += tau_NKATP;
			// 	current_time_NKATP_AUX += tau_NKATP;
			// 	// cout << setprecision(10) << "Tau NKP: " << tau_NKATP << endl;
			// 	// cout << setprecision(10) << "Tau MAX: " << tau_max << endl;
			// }

			/* CP Reaction */
			// if (current_time_CP < tau_max) 
			// {
			// 	choice = gillespie.CP_reaction();
			// 	tau_CP = choice[4];

			// 	// cout << "Ca pump functioning..." << endl;
			// 	for (int i = 0; i < 0*num_reactions; i++)
			// 	{
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "C", -ALPHA);
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "C_o", ALPHA);

			// 		tecido.accumulate(choice[1], choice[2], choice[3], "ATP", -ALPHA);
			// 		tecido.accumulate(choice[1], choice[2], choice[3], "ADP", ALPHA);
			// 	}
			// 	E_noise = E_noise + pow(ALPHA,2);

			// 	current_time_CP += tau_CP;
			// 	current_time_CP_aux += tau_CP;
			// 	// cout << setprecision(10) << "Tau CP: " << tau_CP << endl;
			// }

			/* Getting Tau Max and Updating time for new iteration */
			//cout << setprecision(5) << "C = " << tecido.get(choice[1], choice[2], choice[3], "C") << endl;

			if (tau_calcium == 0) //(tau_NCX == 0 ||  || tau_CP == 0 || tau_NKATP == 0) //  || tau_sodium_inter == 0 
			{
				cout << "Ca reactions didn\'t happen!" << endl;
				break;
			}

			// Modulation - Begin
			if (current_time_modulation >= time_slot)
			{
				// cout << setprecision(5) << "Current time modulation: " << current_time_modulation << endl;
				// cout << setprecision(5) << "time_slot: " << time_slot << endl;
				// cout << setprecision(5) << "C_variation TX: " << tecido.get(tx_x, tx_y, tx_z, "C_variation") << endl;

				// Transmitter
				Tx_states.push_back(tecido.modulation(tx_x, tx_y, tx_z));
				
				tecido.rxtx_concentration["tx"].push_back(tecido.get(tx_x, tx_y, tx_z, "C_variation"));
				tecido.rxtx_concentration["rx"].push_back(tecido.get(tx_x + destination, tx_y, tx_z, "C_variation"));				

				tecido.set(tx_x, tx_y, tx_z, "C_variation", 0);
				tecido.set(tx_x + destination, tx_y, tx_z, "C_variation", 0);

				current_time_modulation = 0;
			}
			// Modulation - End

			if (tau_flag == false)
			{
				tau_max  = max(tau_calcium, max(tau_calcium_extra, max(tau_sodium_inter, max(tau_sodium_extra, max(tau_NCX, max(tau_NKATP, tau_CP))))));
				tau_flag = true;
				current_time += tau_max*1000;

				if (current_time > 210)
				{
					if (greaterThanTimeScounter++ > 10)
					{
						current_time = 200;
						break;
					}

					current_time -= tau_max*1000;

					tau_flag = false;
					tau_max  = 100000;
				}
			}
			
			/* STORAGE OF CALCIUM CONCENTRATION */
			C_tx.push_back(tecido.get(tx_x, tx_y, tx_z, "C"));
			C_rx.push_back(tecido.get(tx_x + destination, tx_y, tx_z, "C"));

			// || current_time_NKATP < tau_max || current_time_CP < tau_max
		} while (current_time_calcium < tau_max || current_time_sodium_inter<tau_max || current_time_NCX < tau_max);

		// /* DIFFUSION ERROR => NO INCREMENT TIME */
		// if (tau_calcium == 0) current_time -= (tau_max * 1000); //(tau_NCX == 0 &&  && tau_CP == 0)  && tau_sodium_inter == 0 && tau_NKATP == 0 
	}

	/* ### CALCULATING GAIN AND SNR ### */

	double acc_c_tx = accumulate(C_tx.begin(), C_tx.end(), 0.0);
	double acc_c_rx = accumulate(C_rx.begin(), C_rx.end(), 0.0);

	// cout << "E_signal = " << E_signal << "; E_noise = " << E_noise << endl;

	double calc_SNR = 10 * log10(E_signal / E_noise);
	double calc_gain = 10 * log10((acc_c_rx / C_rx.size()) / ((acc_c_tx) / C_tx.size()));

	/* ### END GAIN AND SNR ### */

	/* ### DEMODULATION - BEGIN ### */

	int Tx_id 				 = tecido.getId(tx_x,tx_y,tx_z); 
	int Rx_id 				 = tecido.getId(tx_x + destination,tx_y,tx_z);
	int time_size 			 = Tx_states.size();
	int Rx_states[time_size] = {0};
	
	tecido.demodulation(Rx_states, Tx_id, Rx_id, total_reactions, time_size);

	// cout << "Rx States: ";
	// for (int k = 0; k < time_size; k++)
	// {
	// 	cout << Rx_states[k];
	// }
	// cout << endl;

	/* ### DEMODULATION - END ### */

	/* ### CALCULATING CHANNEL CAPACITY AND BER - BEGIN ### */

	int bit_number = 2, time_slots_number = destination, contc=0, contb=0;
	int rx_x = tx_x + destination;
	
	double px[bit_number] = {}, py[bit_number] = {}, pyx_joint[bit_number][bit_number] = {0}, BER_counter = 0;
	double var, mean_channel_capacity, mean_BER, I_xy = 0;
	
	vector<double> channel_capacity, BER;

	// cout << "Tx: ";

	// for (int j = 0; j < time_size; j++)
	// {
	// 	cout << Tx_states[j]; 
	// }
	// cout << endl;
	
	// cout << "Time_slot_number: " << time_slots_number << endl;
	
	/* Distribuição de probabilidade do Tx*/
	px[1] = accumulate(Tx_states.begin(), Tx_states.end(), 0.0) / time_size;
	
	if (px[1] == 1)
		px[1] = 0.999999999;
	
	px[0] = 1 - px[1];

	py[1] = accumulate(Rx_states, Rx_states + time_size, 0.0) / time_size;
	
	if (py[1] == 1)
		py[1] = 0.999999999;
	else if (py[1] == 0)
		py[1] = 0.0000000001;
	
	py[0] = 1 - py[1];

	for (int i = 0; i < time_size; i++)
	{
		if (Rx_states[i] == 1 && Tx_states[i] == 0)
		{
			pyx_joint[ Rx_states[i] ][ Tx_states[i] ]++;
		}

		if ( Rx_states[i] != Tx_states[i] )
		{
			BER_counter++;
		}
	}

	pyx_joint[1][0] = pyx_joint[1][0] / time_size;

	if (pyx_joint[1][0] == 0)
		pyx_joint[1][0] = 0.0000000001;
	else if (pyx_joint[1][0] == 1)
		pyx_joint[1][0] = 0.999999999;
	
	pyx_joint[0][0] = 1 - pyx_joint[1][0];
	pyx_joint[1][1] = py[1] + pyx_joint[1][0] - (py[1] * pyx_joint[1][0]);

	if (pyx_joint[1][1] == 0)
		pyx_joint[1][1] = 0.0000000001;
	else if (pyx_joint[1][1] == 1)
		pyx_joint[1][1] = 0.999999999;

	pyx_joint[0][1] = 1 - pyx_joint[1][1];

	for (int y = 0; y < bit_number; y++) // Number of y1 given x0; Number of y1 given x1; Number of y0 given x0; Number of y0 given x1
	{	
		for (int x = 0; x < bit_number; x++)
		{
			I_xy = I_xy + px[x] * pyx_joint[y][x] * log2( pyx_joint[y][x] / py[y] ); // Mutual Information
		}
	}

	mean_BER 			  = BER_counter / time_size;
	mean_channel_capacity = I_xy;

	/* ### CALCULATING CHANNEL CAPACITY AND BER - END ### */

	/* DUMP RESULTS */
	
	// cout << "Saving results..." << endl;

	file_results << frequency << "," << destination << "," << time_slot << "," << calc_gain <<  "," << calc_SNR << "," << mean_channel_capacity << "," << mean_BER << ",\n";
	
	// cout << setprecision(5) << "cc = " << mean_channel_capacity << "; BER = " << mean_BER << endl;

};

/* MAIN */
int main()
{
	int simulation_number = 10;
	vector<double> time_slot{0.0005,0.0006,0.0007,0.0008,0.0009,0.001,0.002,0.003,0.004,0.005}; //s
	vector<double> frequencies{0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1}; //[Hz]
	string topology = "RD";
	// int destination = 1;

	// Mean results
	ofstream file_results;
	string fileName = "Na_NCX";

	cout << "File Name: " << fileName << ", Simulation Number: ";

	file_results.open("results/" + fileName + ".csv");
	file_results << "Freq (Hz),Range,Time Slot (s),Gain (dB),SNR (dB),Channel Capacity (bits),BER,\n";

	for (int j = 0; j < simulation_number; j++)
	{
		cout << j << endl;

		for (double frequency : frequencies)
		{
			for (int destination = 1; destination < 7; destination++)
			{
				for (double time:time_slot)
				{
					file_results.open ("results/" + fileName + ".csv", ios::app); 

					simulation(destination, frequency, topology, time, file_results, fileName);

					file_results.close();
				}
			}
		}
	}
	

	return 0;
};