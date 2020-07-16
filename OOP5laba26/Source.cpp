#include <windows.h> // ���������� �������� � ��������� API
#include "ctime"
#include "resource.h"
// �������� ����:
HINSTANCE hInst; 	//���������� ��������	
LPCTSTR szWindowClass = "QWERTY";
LPCTSTR szTitle = "������������ 5 ��� 26 �������";
SHORT xnew, ynew;
// ��������� ���� �������
HICON hI1, hI2;
HCURSOR hC;
//bool shot=false;
int vero = 1000;
bool YouWin = false;
bool SpawnBoss = false;
bool GameOver = false;
int BorderX = GetSystemMetrics(SM_CXSCREEN);
int BorderY = GetSystemMetrics(SM_CYSCREEN);
int indexGun = 1;
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
class Gun
{
public:
	int speed;
	int x, y;
	int damage;
	int sizeGun;
	bool enabled = true;
	Gun() {}
	virtual void Shot(HDC &hdc) = 0;
};
class Digl : public Gun
{
public:
	Digl()
	{
		speed = 10;
		x = xnew;
		y = BorderY - 80 - 50;
		damage = 50;
		sizeGun = 5;
	}
	virtual void Shot(HDC &hdc) override
	{
		if (!enabled) return;
		HBRUSH ColorGun;
		ColorGun = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(hdc, ColorGun);
		y = y - speed;
		RoundRect(hdc, x - sizeGun, y + sizeGun, x + sizeGun, y - sizeGun, 2000, 2000);
		DeleteObject(ColorGun);
		if (y < 0)
			enabled = false;
	}
};
class Bazooka : public Gun
{
public:
	Bazooka()
	{
		speed = 10;
		x = xnew;
		y = BorderY - 80 - 50;
		damage = 120;
		sizeGun = 10;
	}
	virtual void Shot(HDC &hdc) override
	{
		if (!enabled) return;
		HBRUSH ColorGun;
		ColorGun = CreateSolidBrush(RGB(255, 0, 0));
		SelectObject(hdc, ColorGun);
		y = y - speed;
		RoundRect(hdc, x - sizeGun, y + sizeGun, x + sizeGun, y - sizeGun, 2000, 2000);
		DeleteObject(ColorGun);
		if (y < 0)
			enabled = false;
	}
};
class Konteiner
{
public:
	int number;
	Gun **pointer;
	Konteiner()
	{
		this->number = 0;
		pointer = nullptr;
	}
	void Add(Gun &Temp)
	{
		Gun **lol;
		lol = pointer;
		pointer = new Gun*[++number];
		for (int i = 0; i < number - 1; i++)
			pointer[i] = lol[i];
		pointer[number - 1] = &Temp;
		delete[] lol;
	}
	void Delite(Gun &Temp)
	{
		Gun **lol;
		lol = pointer;
		int indx;
		for (int i = 0; i < number; i++)
		{
			if (pointer[i] == &Temp)
			{
				indx = i;
				break;
			}
		}
		pointer = new Gun*[--number];
		for (int i = 0; i < indx; i++)
			pointer[i] = lol[i];
		for (int i = indx + 1; i < number + 1; i++)
			pointer[i - 1] = lol[i];
		delete[] lol;
	}
	void DeliteAll()
	{
		number = 0;
		delete[] pointer;
		pointer = nullptr;
	}
	void Graphic(HDC &hdc)
	{
		if (number == 0) return;
		for (int i = 0; i < number; i++)
			pointer[i]->Shot(hdc);
	}
};
Konteiner konteiner;
Digl *digl;
Bazooka *bazooka;
Gun *gun = new Digl;
class Player {
public:
	double health = 10;
	Gun *GunPalm = new Digl;
	bool enabled = true;
	int x;
	int y = BorderY - 80;
	int Xdeath;
	double koff = health / 150;
	void Shoot(Gun &gun)
	{
		konteiner.Add(gun);
	}
	void damage(int x)
	{
		health -= x;
	}
	void Show(HDC &hdc)
	{
		if (health <= 0)
		{
			GameOver = true;
			Xdeath = xnew;
			TextOut(hdc, BorderX / 2, BorderY / 2, "�� ���������", strlen("�� ���������"));
		}
		HBRUSH PColor;
		x = xnew;
		PColor = CreateSolidBrush(RGB(0, 255, 255));
		SelectObject(hdc, PColor);
		Rectangle(hdc, x - 25, y, x + 25, y - 50);
		DeleteObject(PColor);
		PColor = CreateSolidBrush(RGB(255, 0, 0));
		SelectObject(hdc, PColor);
		Rectangle(hdc, 10, BorderY / 2, 25, (BorderY / 2) - (health / koff));
		TextOut(hdc, 10, BorderY / 2 + 10, "hp", strlen("HP"));
		DeleteObject(PColor);
	}
};
Player player;
class Drop
{
public:
	int x, y;
	int size;
	int Bspeed;
	bool enabled = true;
	int GunActivated;
	int damage;
	Drop() {}
	virtual void AirLife(HDC &hdc) = 0;
};
class DropMine : public Drop
{
public:
	DropMine(int _x, int _y)
	{
		size = 10;
		Bspeed = 6;
		x = _x;
		y = _y;
		damage = 2;
		GunActivated = 0;
	}
	virtual void AirLife(HDC &hdc) override
	{
		if (!enabled) return;
		HBRUSH krolik;
		krolik = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(hdc, krolik);
		y += Bspeed + 2;
		RoundRect(hdc, x, y, x + size, y + size, 20, 20);
		DeleteObject(krolik);
		if (y > BorderY) enabled = false;
		//SelectObject(hdc, b1);
	}
};
class DropGun : public Drop
{
	int item;
public:
	DropGun(int _x, int _y)
	{
		x = _x;
		y = _y;
		size = 20;
		Bspeed = 5;
		item = rand() % 3;
		damage = 0;
		switch (item)
		{
		case 0:
			GunActivated = 2;
			break;
		case 1:
			GunActivated = 1;
			break;
		case 2:
			GunActivated = 3;
			break;
		}
	}
	virtual void AirLife(HDC &hdc) override
	{
		if (!enabled) return;
		HBRUSH krolik;
		krolik = CreateSolidBrush(RGB(255, 0, 255));
		DeleteObject(krolik);
		switch (item)
		{
		case 0:
			krolik = CreateSolidBrush(RGB(255, 0, 255));
			SelectObject(hdc, krolik);
			break;
		case 1:
			krolik = CreateSolidBrush(RGB(157, 255, 50));
			SelectObject(hdc, krolik);
			break;
		case 2:
			krolik = CreateSolidBrush(RGB(255, 255, 0));
			SelectObject(hdc, krolik);
			break;
		}
		y += Bspeed;
		RoundRect(hdc, x, y, x + size, y + size, 20, 20);
		DeleteObject(krolik);
		if (y > BorderY) enabled = false;
		//SelectObject(hdc, b1);
	}
};
class KonteinerDrop
{
public:
	int number;
	Drop **pointer;
	KonteinerDrop()
	{
		this->number = 0;
		pointer = nullptr;
	}
	void Add(Drop &Temp)
	{
		Drop **lol;
		lol = pointer;
		pointer = new Drop*[++number];
		for (int i = 0; i < number - 1; i++)
			pointer[i] = lol[i];
		pointer[number - 1] = &Temp;
		delete[] lol;
	}
	void Delite(Drop &Temp)
	{
		Drop **lol;
		lol = pointer;
		int indx;
		for (int i = 0; i < number; i++)
			if (pointer[i] == &Temp)
			{
				indx = i;
				break;
			}
		pointer = new Drop*[--number];
		for (int i = 0; i < indx; i++)
			pointer[i] = lol[i];
		for (int i = indx + 1; i < number + 1; i++)
			pointer[i - 1] = lol[i];
		delete[] lol;
	}
	void DeliteAll()
	{
		number = 0;
		delete[] pointer;
		pointer = nullptr;
	}
	void Graphic(HDC &hdc)
	{
		if (number == 0) return;
		for (int i = 0; i < number; i++)
			pointer[i]->AirLife(hdc);
	}
	void Touch()
	{
		for (int i = 0; i < number; i++)
		{
			if (!pointer[i]->enabled)
			{
				Delite(*pointer[i]);
				continue;
			}
			if ((pointer[i]->x + pointer[i]->size / 2 >= player.x - 25) &&
				(pointer[i]->x + pointer[i]->size / 2 <= player.x + 25) &&
				(pointer[i]->y + pointer[i]->size / 2 >= player.y - 50) &&
				(pointer[i]->y + pointer[i]->size / 2 <= player.y))
			{
				player.damage(pointer[i]->damage);
				if (pointer[i]->GunActivated != 0)
					indexGun = pointer[i]->GunActivated;
				pointer[i]->enabled = false;
			}
		}
	}
};
Drop *drop;
KonteinerDrop KontDrop;
class Monsters
{
public:
	int x, y;
	int size;
	int speed;
	int kolo;
	bool enabled = true;
	Monsters() {}

	virtual void Life(HDC &hdc) {}
	virtual void Damage(Gun &gun) = 0;
};
class Zombie : public Monsters
{
	int health;
public:
	Zombie()
	{
		health = 255;
		size = 21 + rand() % 100;
		x = rand() % (BorderX - size);
		y = 10 + rand() % BorderY / 2;
		speed = rand() % 7;
		kolo = 1 + rand() % 50;
	}
	virtual void Life(HDC &hdc) override
	{
		if (!enabled) return;
		int R = 255 - health;
		int G = health;
		for (int i = 0; i < 255; i++)
			if (R < 255 && G < 255) { R++; G++; }
		HBRUSH krolik;
		krolik = CreateSolidBrush(RGB(R, G, 0));
		SelectObject(hdc, krolik);
		if (x + size + 10 > BorderX || x < 0) speed *= -1;
		x += speed;
		RoundRect(hdc, x, y, x + size, y + size, kolo, kolo);
		DeleteObject(krolik);
		if (rand() % 200 == 1)
		{
			drop = new DropMine(x + rand() % size, y + size);
			KontDrop.Add(*drop);
		}
	}
	virtual void Damage(Gun &gun) override
	{
		health -= gun.damage * 40 / size;
		if (health <= 0)
		{
			enabled = false;
			kolo = rand() % 5;
			if (kolo == 0 || kolo == 1)
			{
				drop = new DropGun(x + size / 2, y);
				KontDrop.Add(*drop);
			}
		}
	}
};
class Boss : public Monsters
{
	int Napr = 1;
	int health;
public:
	Boss()
	{
		health = 510;
		size = 510;
		x = rand() % (BorderX - size);
		y = 10;
		speed = 1;
		kolo = 0;
	}
	virtual void Life(HDC &hdc) 
	{
		if (!enabled) return;
		int R = 255 - health / 2;
		int G = health / 2;
		for (int i = 0; i < 255; i++)
			if (R < 255 && G < 255) { R++; G++; }
		HBRUSH krolik;
		krolik = CreateSolidBrush(RGB(R, 0, G));
		SelectObject(hdc, krolik);
		if (x + size + 10 > BorderX || x < 0) Napr *= -1;
		speed = ((1000 - (10 * (health / 2 * 100 / 255))) / 100) * Napr;
		x += speed;
		size = 50 + health / 2;
		RoundRect(hdc, x, y, x + size, y + size, kolo, kolo);
		DeleteObject(krolik);
		krolik = CreateSolidBrush(RGB(0, 0, 255));
		SelectObject(hdc, krolik);
		Rectangle(hdc, 35, BorderY / 2, 50, (BorderY / 2) - (health / 2 / 1.7));
		TextOut(hdc, 35, BorderY / 2 + 10, "hp", strlen("HP"));
		DeleteObject(krolik);
		if (rand() % 30 == 1)
		{
			drop = new DropMine(x + rand() % size, y + size);
			KontDrop.Add(*drop);
		}
	}
	virtual void Damage(Gun &gun) override
	{
		health -= indexGun;
		if (health <= 0)
		{
			YouWin = true;
			enabled = false;
			kolo = rand() % 10;
			if (kolo == 0 || kolo == 1)
			{
				drop = new DropGun(x + size / 2, y);
				KontDrop.Add(*drop);
			}
		}
	}
};
class KonteinerMonsters
{
public:
	int number;
	Monsters **pointer;
	KonteinerMonsters()
	{
		this->number = 0;
		pointer = nullptr;
	}
	void Add(Monsters &Temp)
	{
		Monsters **lol;
		lol = pointer;
		pointer = new Monsters*[++number];
		for (int i = 0; i < number - 1; i++)
			pointer[i] = lol[i];
		pointer[number - 1] = &Temp;
		delete[] lol;
	}
	void Delite(Monsters &Temp)
	{
		Monsters **lol;
		lol = pointer;
		int indx;
		for (int i = 0; i < number; i++)
			if (pointer[i] == &Temp)
			{
				indx = i;
				break;
			}
		pointer = new Monsters*[--number];
		for (int i = 0; i < indx; i++)
			pointer[i] = lol[i];
		for (int i = indx + 1; i < number + 1; i++)
			pointer[i - 1] = lol[i];
		delete[] lol;
	}
	void DeliteAll()
	{
		number = 0;
		delete[] pointer;
		pointer = nullptr;
	}
	void Graphic(HDC &hdc)
	{
		if (number == 0) return;
		for (int i = 0; i < number; i++)
			pointer[i]->Life(hdc);
	}
	void Touch(Konteiner &KontGun)
	{
		if (number == 0 && !YouWin)
		{
			SpawnBoss = true;
			//vero = 1;  // ���������� ������ �������� �����
			//YouWin = true;
			return;
		}
		for (int i = 0; i < KontGun.number; i++)
		{
			if (!KontGun.pointer[i]->enabled)
			{
				konteiner.Delite(*KontGun.pointer[i]);
				continue;
			}
			for (int j = 0; j < number; j++)
			{
				if (!pointer[j]->enabled)
				{
					Delite(*pointer[j]);
					continue;
				}
				if ((KontGun.pointer[i]->x + KontGun.pointer[i]->sizeGun / 2 >= pointer[j]->x) &&
					(KontGun.pointer[i]->x + KontGun.pointer[i]->sizeGun / 2 <= pointer[j]->x + pointer[j]->size) &&
					(KontGun.pointer[i]->y + KontGun.pointer[i]->sizeGun / 2 >= pointer[j]->y) &&
					(KontGun.pointer[i]->y + KontGun.pointer[i]->sizeGun / 2 <= pointer[j]->y + pointer[j]->size))
				{
					KontGun.pointer[i]->enabled = false;
					pointer[j]->Damage(*KontGun.pointer[i]);
				}
			}
		}
	}
};
Zombie *zombie;
Boss *boss;
KonteinerMonsters konteinermonsters;
// ������� �������� 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;
	// ��������� ����� ���� 
	MyRegisterClass(hInstance);
	// ��������� ���� ��������
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// ���� ������� ����������
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0; 		//����� ����
	wcex.lpfnWndProc = (WNDPROC)WndProc; 		//������ ���������
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 			//���������� ��������
	wcex.hIcon = LoadIcon(NULL, IDI_HAND); 		//���������� ������
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS); 	//���������� �������
	wcex.hbrBackground = CreateSolidBrush(RGB(135, 206, 235)); //��������� ����
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); 				//���������� ����
	wcex.lpszClassName = szWindowClass; 		//��� �����
	wcex.hIconSm = NULL;
	return RegisterClassEx(&wcex); 			//��������� ����� ����
}
// FUNCTION: InitInstance (HANDLE, int)
// ������� ���� �������� � ������ ���������� �������� � ����� hInst
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //������ ���������� ������� � ����� hInst
	hWnd = CreateWindow(szWindowClass, 	// ��� ����� ����
		szTitle, 				// ����� ��������
		WS_OVERLAPPEDWINDOW,			// ����� ����
		CW_USEDEFAULT, 			// ��������� �� �	
		CW_USEDEFAULT,			// ��������� �� Y	
		BorderX, 			// ����� �� �
		BorderY, 			// ����� �� Y
		NULL, 					// ���������� ������������ ����	
		NULL, 					// ���������� ���� ����
		hInstance, 				// ���������� ��������
		NULL); 				// ��������� ���������.
	SetTimer(hWnd, 1, 0, NULL);
	srand(time(0));
	for (int i = 0; i < 10; i++)
	{
		zombie = new Zombie;
		konteinermonsters.Add(*zombie);
	}
	if (!hWnd) 	//���� ���� �� ���������, ������� ������� FALSE
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow); 		//�������� ����
	UpdateWindow(hWnd); 				//������� ����
	return TRUE;
}
// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// ³����� ���������. ������ � �������� �� �����������, �� ��������� � �������
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	static TCHAR ch;
	RECT rt;
	TEXTMETRIC lptm;
	HDC hdc;
	//hI1 = LoadIcon(NULL, IDI_APPLICATION);
	switch (message)
	{
	case WM_CREATE: 				//����������� ��������� ��� �������� ��-��
		break;
	case WM_CHAR:
		ch = (TCHAR)wParam;
		if (ch == '1')
			indexGun = 1;
		if (ch == '2')
			indexGun = 2;
		if (ch == '3')
			indexGun = 3;
		if (ch == '4')
			indexGun = 4;
		if (ch == '0')
		{
			YouWin = false;
			zombie = new Zombie;
			konteinermonsters.Add(*zombie);
		}
		if (ch == '8')
		{
			YouWin = false;
			boss = new Boss;
			konteinermonsters.Add(*boss);
			SpawnBoss = false;
		}
		if (ch == '9')
		{
			player.health = 10;
			GameOver = false;
		}
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT: 				//������������ ����
		hdc = BeginPaint(hWnd, &ps); 	//������ ��������� ����	
		GetClientRect(hWnd, &rt); 		//������� ���� ��� ���������
		if (SpawnBoss)
		{
			boss = new Boss;
			konteinermonsters.Add(*boss);
			SpawnBoss = false;
		}
		konteinermonsters.Touch(konteiner);
		konteiner.Graphic(hdc);
		konteinermonsters.Graphic(hdc);
		KontDrop.Graphic(hdc);
		KontDrop.Touch();
		player.Show(hdc);
		if (YouWin)
		{
			konteinermonsters.DeliteAll();
			TextOut(hdc, BorderX / 2, BorderY / 2, "������!", strlen("������!"));
		}

		EndPaint(hWnd, &ps); 		//�������� ��������� ����	
		break;
	case WM_DESTROY: 				//���������� ������
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		switch (indexGun)
		{
		case 3:
			if (rand() % 4 == 1)
			{
				player.GunPalm = new Digl;
				konteiner.Add(*player.GunPalm);
			}
			break;
		case 4:
			player.GunPalm = new Bazooka;
			konteiner.Add(*player.GunPalm);
			break;
		}
		if (!GameOver)
		{
			if (rand() % vero == 10)
			{
				zombie = new Zombie;
				konteinermonsters.Add(*zombie);
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_LBUTTONDOWN:
		switch (indexGun)
		{
		case 1:
			player.GunPalm = new Digl;
			konteiner.Add(*player.GunPalm);
			break;
		case 2:
			player.GunPalm = new Bazooka;
			konteiner.Add(*player.GunPalm);
			break;
		}
		break;
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == ID_40001)
		{
			konteinermonsters.DeliteAll();
			KontDrop.DeliteAll();
			konteiner.DeliteAll();
			for (int i = 0; i < 10; i++)
			{
				zombie = new Zombie;
				konteinermonsters.Add(*zombie);
			}
			player.health = 10;
			indexGun = 1;
			YouWin = false;
			GameOver = false;
		}
	}
	case WM_MOUSEMOVE:
		xnew = LOWORD(lParam);
		ynew = HIWORD(lParam);
		break;
	default:
		//������� ����������, �� �� �������� ������������
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
