#include <bits/stdc++.h>
#include <fstream>
#include <string>
using namespace std;
void TaoFile(vector<fstream> &f, vector <fstream> &g, int n)
{
	string s;
	for (int i = 0; i < n; i++)
	{
		s = "f" + to_string(i) + ".bin";
		f[i].open(s, ios::in | ios::app | ios::binary);
		f[i].close();
		s = "g" + to_string(i) + ".bin";
		g[i].open(s, ios::in | ios::app | ios::binary);
		g[i].close();
	}
}
void CopyBufferSangFile(double *buffer, vector <fstream> &f, int &i, int n, double &socuoi, int &sorun, int sl)
{
	int itj = 0, iti = 0;
	double chan = -1; //ghi vao file danh dau ket thuc 1 run
	string s;
	int csl = 0;
	double *tmp = new double[64];
	while (itj < 64 && csl < sl)
	{
		while (itj < 64)
		{
			if (buffer[itj] < buffer[itj + 1]) itj++; else break; // xac dinh vi tri dau va cuoi cua 1 run trong buffer
		}
		for (int j = iti; j <= itj; j++) tmp[j - iti] = buffer[j]; // chuyen buffer sang mang tmp de de dang write vao file
		csl += itj - iti + 1;
		socuoi = buffer[itj];
		//s = "f" + to_string(i) + ".bin"; // ten file
		//f[i].open(s, ios::out | ios::binary);
		f[i].write(reinterpret_cast<char *>(tmp), (itj - iti + 1) * 8);
		sorun++;
		iti = itj + 1; itj++;
		if (iti <= 64 && csl <= sl) // buffer chua het
		{
			f[i].write(reinterpret_cast<char *>(&chan), 8);
			//f[i].close();
		}
		i = (i + 1) % n; // xong 1 run thi write o file tiep theo
	}
	delete []tmp;
}
void ChiaRunTuFiSangNguon(vector <fstream> &f, ifstream &fi, int &n, int &sorun)
{
	fi.open("E:/CTDL NC/Code/Test/test7.bin", ios::in | ios::binary);
	fi.seekg(0, fi.end); // tinh length cua file binary
	unsigned long long int length = fi.tellg();
	fi.seekg(0, fi.beg);
	double *buffer = new double[64]; // tao buffer 512 byte = 64 double 8 byte
	unsigned long long int du = length % 512; unsigned long long int nguyen = length / 512; // tinh so lan doc nguyen buffer va phan du con lai
	int i = -1; // file f[i] de write run vao
	unsigned long long int sl = length / 8; // sl = so luong phan tu trong file input
	double socuoi = -1; // ghi lai so cuoi trong buffer trc de so sanh voi so dau cua buffer hien tai, tu do xem xet co con run hay khong
	double chan = -1;
	string s;
	for (int j = 0; j < n; j++)
	{
		s = "f" + to_string(j) + ".bin"; // ten file
		f[j].open(s, ios::out | ios::binary);
	}
	while (!fi.eof())
	{
		if (nguyen > 0)
		{
			fi.read(reinterpret_cast<char *>(buffer), 512);
			nguyen--;
			if (buffer[0] > socuoi) // quan ly de co the tiep tuc write vao file trc do khi run chua het
			{
				if (i == -1) i = 0;
				else if (i == 0)
				{
					i = n - 1;
					sorun--;
				}
				else if (i > 0)
				{
					i--;
					sorun--;
				}
			}
			CopyBufferSangFile(buffer, f, i, n, socuoi, sorun, 64);
		}
		else
		{
			fi.read(reinterpret_cast<char *>(buffer), du + 1);
			buffer[du / 8] = -1;
			if (buffer[0] > socuoi)
			{
				if (i == -1) i = 0;
				else if (i == 0)
				{
					i = n - 1;
					sorun--;
				}
				else if (i > 0)
				{
					i--;
					sorun--;
				}
			}
			CopyBufferSangFile(buffer, f, i, n, socuoi, sorun, du / 8);
		}
	}
	if (i > 0) i--; else i = n - 1;
	f[i].write(reinterpret_cast<char *>(&chan), 8);
	for (int j = 0; j < n; j++)
	{
		f[j].close();
	}
	fi.close();
	delete []buffer;
}
struct read // struct nay ho tro viec doc file nguon
{
	unsigned long long int nguyen, du;
};
void Docfile(fstream &file, double ** buffer, int i, vector<read> &a, int *it)
{
	if (!file.eof())
	{
		if (a[i].nguyen > 0)
		{
			file.read(reinterpret_cast<char *>(buffer[i]), 512);
			a[i].nguyen--;
		}
		else
		{
			file.read(reinterpret_cast<char *>(buffer[i]), a[i].du + 1);
			buffer[i][a[i].du / 8] = -2; // vi tri tiep theo se la -2 de danh dau het file
		}
		it[i] = 0; // quay vi tri cua it[i] ve ban dau la 0
	}
	else
	{
		buffer[i][0] = -2; // danh dau la da het file
	}
}
void Merge(vector<fstream> &ng, vector<fstream> &d, int &sorun, int n, int ngd)
{
	double **buffer = new double *[n];
	for (int i = 0; i < n; i++) buffer[i] = new double[64];
	vector<read> a(n); // tao mang a de quan ly doc file nguon (nguyen va du)
	string s;
	string sng, sdich;
	if (ngd == 0)
	{
		sng = "f";
		sdich = "g";
	}
	else
	{
		sng = "g";
		sdich = "f";
	}
	for (int i = 0; i < n; i++) // mo va xac dinh length cua file nguon
	{
		s = sng + to_string(i) + ".bin"; // ten file
		ng[i].open(s, ios::in | ios::binary);
		ng[i].seekg(0, ng[i].end); // tinh length cua file binary
		unsigned long long int length = ng[i].tellg();
		ng[i].seekg(0, ng[i].beg);
		a[i].du = length % 512; a[i].nguyen = length / 512; // tinh so lan doc nguyen buffer va phan du con lai
	}
	for (int i = 0; i < n; i++) // mo file dich
	{
		s = sdich + to_string(i) + ".bin"; // ten file
		d[i].open(s, ios::out | ios::binary | ios::trunc);

	}
	for (int i = 0; i < n; i++) // mac dinh doc lan dau tien
	{
		if (!ng[i].eof())
		{
			if (a[i].nguyen > 0)
			{
				ng[i].read(reinterpret_cast<char *>(buffer[i]), 512);
				a[i].nguyen--;
			}
			else
			{
				ng[i].read(reinterpret_cast<char *>(buffer[i]), a[i].du + 1);
				if (a[i].du == 0) buffer[i][0] = -1;
			}
		}
	}
	struct trunggian // struct nay la queue de push phan tu tu buffer[i] vao va lay ra phan tu nho nhat ma biet duoc phan tu do thuoc buffer nao
	{
		double data;
		int i;
		bool operator<(const trunggian& rhs) const
		{
			return data > rhs.data;
		}
	};
	priority_queue<trunggian> minheap; // min priority de push phan tu cua tung buffer vao chon ra phan tu nho nhat
	int sofileconlai = n;
	int checkrun = n; // kiem tra xem da merge du run vao file dich chua de den file dich tiep theo
	trunggian tmp;
	int *it = new int[n]; // luu vi tri hien tai cua cac phan tu trong run hien hanh
	for (int i = 0; i < n; i++) it[i] = 0;
	for (int i = 0; i < n; i++)// mac dinh khoi tao push lan dau tien vao minheap
	{
		if (buffer[i][0] != -1) // neu ngay tu dau la -1 nghia la file trong khong con run nua
		{
			tmp.data = buffer[i][0];
			tmp.i = i;
			minheap.push(tmp);
			it[i]++;
		}
		else
		{
			buffer[i][0] = -2; // danh dau la da het file
			sofileconlai--;
			checkrun--;
		}
	}
	double *merge = new double[64]; // de luu cac so sau khi merge va write vao file dich
	int imerge = 0; // danh dau vi tri hien tai cua mang merge
	sorun = 0;
	int idich = 0; // danh dau vi tri file dich hien tai
	double chan = -1;
	while (sofileconlai > 0) // da merge het tu file nguon sang file dich
	{
		while (checkrun > 0)
		{
			tmp = minheap.top();
			minheap.pop();
			merge[imerge] = tmp.data;
			imerge++;
			tmp.data = buffer[tmp.i][it[tmp.i]];
			if (tmp.data > -1)
			{
				minheap.push(tmp);
				it[tmp.i]++;
				if (it[tmp.i] >= 64)
				{
					Docfile(ng[tmp.i], buffer, tmp.i, a, it);
				}
			}
			else checkrun--;
			if (imerge >= 64) // neu da full mang merge thi write vao file
			{
				d[idich].write(reinterpret_cast<char *>(merge), 512);
				imerge = 0; // quay imerge lai ve vi tri 0 ban dau
			}
			else if (minheap.empty())
			{
				d[idich].write(reinterpret_cast<char *>(merge), imerge * 8);
				imerge = 0; // quay imerge lai ve vi tri 0 ban dau
			}
		}
		d[idich].write(reinterpret_cast<char *>(&chan), 8);
		checkrun = n;
		sofileconlai = n;
		for (int i = 0; i < n; i++)
		{
			if (buffer[i][0] == -2)
			{
				checkrun--;
				sofileconlai--;
			}
			else
			{
				it[i]++;
				if (it[i] < 64)
				{
					if (buffer[i][it[i]] == -2)
					{
						buffer[i][0] = -2;
						checkrun--;
						sofileconlai--;
					}
					else
					{
						tmp.data = buffer[i][it[i]];
						tmp.i = i;
						if (tmp.data > -1)
						{
							minheap.push(tmp);
							it[i]++;
							if (it[i] >= 64)
							{
								Docfile(ng[i], buffer, i, a, it); //Doc file
							}
						}
						else
						{
							buffer[i][0] = -2;
							checkrun--;
							sofileconlai--;
						}
					}
				}
				else
				{
					Docfile(ng[i], buffer, i, a, it); //Doc file
					tmp.data = buffer[i][it[i]];
					tmp.i = i;
					if (tmp.data > -1)
					{
						minheap.push(tmp);
						it[i]++;
						if (it[i] >= 64)
						{
							Docfile(ng[i], buffer, i, a, it); //Doc file
						}
					}
					else
					{
						buffer[i][0] = -2;
						checkrun--;
						sofileconlai--;
					}
				}
			}
		}
		sorun++;
		idich = (idich + 1) % n;
	}
	for (int i = 0; i < n; i++)
	{
		ng[i].close();
		d[i].close();
	}
	delete []it;
	delete []merge;
	for (int i = 0; i < n; i++) delete []buffer[i];
	delete []buffer;
}
void CopySangFileOut(fstream &ft, ofstream &fo, int ngd)
{
	string s;
	if (ngd == 1) s = "g0.bin"; else s = "f0.bin";
	ft.open(s, ios::in | ios::binary);
	fo.open("E:/CTDL NC/Code/Output/output7.bin", ios::out | ios::binary | ios::app);
	ft.seekg(0, ft.end); // tinh length cua file binary
	unsigned long long int length = ft.tellg();
	ft.seekg(0, ft.beg);
	double *buffer = new double[64]; // tao buffer 512 byte = 64 double 8 byte
	unsigned long long int du = length % 512; unsigned long long int nguyen = length / 512; // tinh so lan doc nguyen buffer va phan du con lai
	while (!ft.eof())
	{
		if (!ft.eof())
		{
			if (nguyen > 0)
			{
				ft.read(reinterpret_cast<char *>(buffer), 512);
				nguyen--;
				fo.write(reinterpret_cast<char *>(buffer), 512);
			}
			else
			{
				ft.read(reinterpret_cast<char *>(buffer), du + 1);
				fo.write(reinterpret_cast<char *>(buffer), du - 8);
			}
		}
	}
	ft.close();
	fo.close();
	delete []buffer;
}
int main()
{
	const clock_t begin_time = clock();
	int n, sorun = 0;
	ifstream fi;
	ofstream fo;
	cin >> n;
	vector <fstream> f, g; // f la tap nguon; g la tap dich
	f.resize(n); g.resize(n);
	TaoFile(f, g, n); // tao file san de tranh dung cham app va trunc
	ChiaRunTuFiSangNguon(f, fi, n, sorun); // thuc hien chia run tu file input sang n file nguon
	int ngd = 0; // 0: f = nguon, g = dich; 1: g = nguon, f = dich
	while (sorun > 1) // khi run > 1 thi tiep tuc merge
	{
		if (ngd == 0)
		{
			Merge(f, g, sorun, n, ngd);
			ngd = 1;
		}
		else
		{
			Merge(g, f, sorun, n, ngd);
			ngd = 0;
		}
	}
	if (ngd == 0) CopySangFileOut(f[0], fo, ngd); else CopySangFileOut(g[0], fo, ngd);
	std::cout << float(clock() - begin_time) / CLOCKS_PER_SEC;
	system("pause");
	return 0;
}
