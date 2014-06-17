#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <ctime>
#include <string> 
#include <fstream>

using namespace sf;
using namespace std;

struct kart
{
	string mast;
	int dost;
	kart *next;
	Texture tx;
	Sprite sp;
};

struct gol
{
	string info;
	gol *next;
	kart *perv;
};



void add_kart (kart *&kk, string mm, int dd) //создать карту данной масти
{
	if (kk->next == NULL)
	{
		kart *p = new kart;		
		p->mast=mm;
		p->next=NULL;
		p->dost=dd;
		string tmp = mm;
		tmp += to_string(dd);
		tmp += ".bmp";
		Texture tex;
		tex.loadFromFile(tmp);
		p->tx = tex;
		Sprite spr;
		spr.setTexture(p->tx);
		p->sp = spr;

		kk->next=p;
	}
	else add_kart(kk->next,mm,dd);
}

void comp_kolod (kart *&kk1) //создать колоду
{
	for (int i=7; i<15; i++)
		add_kart(kk1,"cherv",i);
	for (int i=6; i<15; i++)
		add_kart(kk1,"pica",i);
	for (int i=6; i<15; i++)
		add_kart(kk1,"bubna",i);
	for (int i=6; i<15; i++)
		add_kart(kk1,"tref",i);
	
}

void ts_kol(kart *&kk) //поменять 2 карты местами (нужно для тасовки колоды)
{
	int m = 0;
	int r = 1;
	
	if ( (kk->next != NULL) && (kk->next->next != NULL))
	{
		if ((rand() % (r+1) + m) != 0)
		{
			kart *tmp = kk->next;
			kk->next = kk->next->next;
			kart *tmp1 = kk->next->next;
			kk->next->next = tmp;
			kk->next->next->next = tmp1;
		}
		ts_kol(kk->next);
	}
}

void tas_kolod(gol *&gg) //перетасовка колоды
{
	int min = 1000;
	int range = 1000;
	int m = 0;
	int r = 1;

	int e = rand() % (range+1) + min;

	for (int i=1; i < e; i++)
	{
		if ((rand() % (r+1) + m) != 0)
		{
			kart *tmp = gg->perv;
			gg->perv = gg->perv->next;
			kart *tmp1 = gg->perv->next;
			gg->perv->next = tmp;
			gg->perv->next->next = tmp1;
		}
		ts_kol(gg->perv);
	}
}

void kozir(kart *&kk, string mm) // выбирает козырь и делает все карты козырями 
{
	if (kk->mast == mm)
		kk->dost += 10;
	if (kk->next != NULL)
		kozir(kk->next, mm);
}

int kol_vo(kart *&kk, int i) // считает количество карт(инициализировать от i=0)
{
	if (kk == NULL)
		return 0;
	if (kk->next == NULL)
		return i+1;
	else return kol_vo(kk->next,i+1);
}

kart* adr(kart *&kk,gol *&gl, int num, int i) //возвращает указатель на элемент num, при этом выставляет NULL(инициализировать от i=1)
{
	if (num == 1)
	{
		kart *tmp;
		tmp = new kart;
		tmp = kk;
		gl->perv = NULL;
		return tmp;
	}
	else	
	if (i+1 == num) 
	{
		kart *tmp;
		tmp = new kart;
		tmp = kk->next;
		kk->next = NULL;
		return tmp;
	}
	else adr(kk->next, gl, num, i+1);
}

kart* adress(kart *&kk,gol *&gl, int n) //возвращает указатель на первую карту из заданного количества
{
	if (kol_vo(kk,0) == 1) return kk;
	else return adr(kk,gl, kol_vo(kk,0) - n+1,1);
}
 
 void srt(kart *&kk) // вспомогательная процедура для сортировки (один проход пузырьком)
	{
		if ((kk->next != NULL) && (kk->next->next != NULL))
		{
			if (kk->next->dost < kk->next->next->dost)
			{
				kart *tmp;
				tmp = new kart;
				tmp = kk->next;
				kart *tmp1;
				tmp1 = new kart;
				tmp1 = kk->next->next->next;
			
				kk->next = kk->next->next;
				kk->next->next = tmp;
				kk->next->next->next = tmp1;
			}
		
			srt(kk->next);}
	}

void sort(gol *&gl) //сортирует список по убыванию (пузырьком)
{
	if (gl->perv->next != NULL)
	{
		for (int i=1; i<kol_vo(gl->perv,0)+1; i++)
		{
			if (gl->perv->dost < gl->perv->next->dost)
			{
				kart *tmp;
				tmp = new kart;
				tmp = gl->perv;
				kart *tmp1;
				tmp1 = new kart;
				tmp1 = gl->perv->next->next;

				gl->perv = gl->perv->next;
				gl->perv->next = tmp;
				gl->perv->next->next = tmp1;
			}
			srt(gl->perv);
		}
	}
		
}

bool check (kart *&kk, int it) // проверка списка карт на наличие указанного достоинства
{
	if (kk == NULL)
		return true;
	int it_tmp = it;
	if (it_tmp > 14)
		it_tmp -= 10;
	int kk_tmp = kk->dost;
	if (kk_tmp > 14)
		kk_tmp -= 10;

	if (kk_tmp == it_tmp) return true;
	if (kk->next != NULL)
		check (kk->next,it);
	else return false;
}

void ppln (kart *&kk,kart *adrs)// вспомогательная процедура для добавления карт
{
	if (kk->next != NULL)
		ppln(kk->next,adrs);
	else kk->next = adrs;
}

void popoln_c (gol *&gl) // пополняет карты игрокa до 6 карт после битты
{
	if (kol_vo(gl->next->perv,0) < 6)
	{
		if (kol_vo(gl->perv,0) > 6-kol_vo(gl->next->perv,0))
			ppln(gl->next->perv,adress(gl->perv,gl,6-kol_vo(gl->next->perv,0)));
		else 
		{
			ppln(gl->next->perv,gl->perv);
			gl->perv = NULL;
		}		
	}
}

void popoln_h (gol *&gl) // пополняет карты игрокa до 6 карт после битты
{
	if (kol_vo(gl->next->next->perv,0) < 6)
	{
		if (kol_vo(gl->perv,0) > 6-kol_vo(gl->next->next->perv,0))
			ppln(gl->next->next->perv,adress(gl->perv,gl,6-kol_vo(gl->next->next->perv,0)));
		else 
		{
			ppln(gl->next->next->perv,gl->perv);
			gl->perv = NULL;
		}		
	}
}

void print(kart *&kk,int i) //вывод колоды
{
	if (kk != NULL)
	{
		cout<<i<<": "<<kk->mast<<" - "<<kk->dost<<endl;
		if (kk->next != NULL)
			print(kk->next,i+1);
	}
}

gol* golova() //подготавливает все к началу игры
{
	kart *hh;
	hh = new kart;
	hh->next=NULL;
	hh->mast="cherv";
	hh->dost=6;
	Texture tex;
	tex.loadFromFile("cherv6.bmp");
	hh->tx = tex;
	Sprite spr;
	spr.setTexture(hh->tx);
	hh->sp = spr;

	gol *p1;
	p1 = new gol;
	p1->info="koloda";
	p1->perv=hh;

	comp_kolod(p1->perv);
	tas_kolod(p1);
	kozir(p1->perv,p1->perv->mast);

	gol *p2;
	p2 = new gol;
	p2->info="g_comp";
	p2->perv=adress(p1->perv,p1,6);
	p1->next=p2;
	sort(p2);
	
	gol *p3;
	p3 = new gol;
	p3->info="g_hum";
	p3->perv=adress(p1->perv,p1,6);
	p2->next=p3;
	sort(p3);

	gol *p4;
	p4 = new gol;
	p4->info="game";
	p4->perv=NULL;
	p3->next=p4;

	gol *p5;
	p5 = new gol;
	p5->info="bita";
	p5->perv=NULL;
	p4->next=p5;

	return p1;
}

kart* adr_nonull(kart *&kk, int num, int i) //возвращает указатель на элемент num, при этом НЕ выставляет NULL(инициализировать от i=1)
	{
		if (num == 1)
			return kk;
		else if (i+1 == num)
			return kk->next;		
		else adr_nonull(kk->next, num, i+1);
	}

kart* adr_vit(kart *&kk, gol *&gl, int num, int i) //вытаскивает одну карту из списка карт(инициализировать от i=1)
{
	if (num == 1)
	{
		kart *tmp;
		tmp = new kart;
		tmp = kk;
		gl->perv = kk->next;
		tmp->next = NULL;
		return tmp;
	}
	else if (i+1 == num)
	{
		kart *tmp;
		tmp = new kart;
		tmp = kk->next;
		kk->next = kk->next->next;
		tmp->next = NULL;
		return tmp;
	}
	else adr_vit(kk->next,gl, num, i+1);
}

kart* posl(kart *&kk) // возвращает указатель на первую карту (последний элемент списка)
{
	if (kk->next == NULL)	
		return kk;

	else posl(kk->next);
}

kart* para(kart *&kk) // вытаскивает из списка карт 2 карты одинакового достоинства
{
	if ((kk->dost<16) && (kk->dost == kk->next->dost))
	{
		kart *tmp;
		tmp = new kart;
		tmp = kk;
		kk->next = kk->next->next;
		return tmp;
	}
}

bool comp_hod(gol *&gl) // ход компьютера
{
	sort(gl->next);
	
	if (gl->perv != NULL) // если колода непустая
	{
		for (int i = kol_vo(gl->next->perv,0)-1; i > 0; i--)	// проверка на наличие пар одного достоинства		
			if ( (adr_nonull(gl->next->perv, i, 1)->dost <12) && (check (gl->next->next->next->perv, adr_nonull(gl->next->perv, i, 1)->dost) == true) && (adr_nonull(gl->next->perv, i, 1)->dost == adr_nonull(gl->next->perv, i, 1)->next->dost ) && (adr_nonull(gl->next->perv, i, 1)->next->dost < 12) )
			{
				if (gl->next->next->next->perv == NULL)
				{
					gl->next->next->next->perv = adr_vit(gl->next->perv, gl->next, i, 1);
					return true;
				}
				else
				{					
					posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
					return true;
				}
			}		
		
		for (int i = kol_vo(gl->next->perv,0); i > 0; i--)	// проверка на минимальную карту не козырь
			if ( (adr_nonull(gl->next->perv, i, 1)->dost <13) && (check (gl->next->next->next->perv, adr_nonull(gl->next->perv, i, 1)->dost) == true) ) 
			{
				if (kol_vo(gl->next->perv,0) == i)
				{
					if (gl->next->next->next->perv == NULL)
					{
						gl->next->next->next->perv = adr(gl->next->perv,gl->next, i, 1);
						return true;
					}
					else
					{
						posl(gl->next->next->next->perv)->next = adr(gl->next->perv,gl->next, i, 1);
						return true;
					}
				}
				else
				{
					if (gl->next->next->next->perv == NULL)
					{
						gl->next->next->next->perv = adr_vit(gl->next->perv, gl->next, i, 1);
						return true;
					}
					else
					{
						posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
						return true;
					}
				}
			}
			
			if (gl->next->next->next->perv == NULL) //  кидает самую маленькую карту если первый ход
			{
				gl->next->next->next->perv = adr(gl->next->perv,gl->next, kol_vo(gl->next->perv,0), 1);
				return true;
			}
				
	}
	else // если колода пустая
	{
		for (int i = kol_vo(gl->next->perv,0)-1; i > 0; i--) //поиск пар
			if ( (adr_nonull(gl->next->perv, i, 1)->dost <14) && (check (gl->next->next->next->perv, adr_nonull(gl->next->perv, i, 1)->dost) == true) && (adr_nonull(gl->next->perv, i, 1)->dost == adr_nonull(gl->next->perv, i, 1)->next->dost ) && (adr_nonull(gl->next->perv, i, 1)->next->dost < 14) ) 
			{
				if (gl->next->next->next->perv == NULL)
				{
					gl->next->next->next->perv = adr_vit(gl->next->perv, gl->next, i, 1);
					return true;
				}
				else
				{					
					posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
					return true;
				}
			}
			
		bool mst = false;
		for (int i = kol_vo(gl->next->perv,0); i > 0; i--)		// поиск карты такой масти которой нет у игрока			
			if ( (adr_nonull(gl->next->perv, i, 1)->dost <15) && (check (gl->next->next->next->perv, adr_nonull(gl->next->perv, i, 1)->dost) == true) )
			{
				for (int j = kol_vo(gl->next->next->perv,0); j > 0; j--)
				{					
					if (adr_nonull(gl->next->perv, i, 1)->mast == adr_nonull(gl->next->next->perv, j, 1)->mast && adr_nonull(gl->next->perv, i, 1)->dost < adr_nonull(gl->next->next->perv, j, 1)->dost)
					{
						mst = true;
						break;
					}
				}

				if (mst == false)
				{
					if (gl->next->next->next->perv == NULL)
					{
						gl->next->next->next->perv = adr_vit(gl->next->perv, gl->next, i, 1);
						return true;
					}
					else
					{					
						posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
						return true;
					}
				}
				mst = false;
			}
						

					
			for (int i = kol_vo(gl->next->perv,0); i > 0; i--)	// проверка на минимальную карту не козырь
				if ( (adr_nonull(gl->next->perv, i, 1)->dost <13) && (check (gl->next->next->next->perv, adr_nonull(gl->next->perv, i, 1)->dost) == true) ) 
					{
					if (kol_vo(gl->next->perv,0) == i)
						{
						if (gl->next->next->next->perv == NULL)
							{
							gl->next->next->next->perv = adr(gl->next->perv,gl->next, i, 1);
							return true;
							}
						else
							{
							posl(gl->next->next->next->perv)->next = adr(gl->next->perv,gl->next, i, 1);
							return true;
							}
						}
					else
						{
						if (gl->next->next->next->perv == NULL)
							{
							gl->next->next->next->perv = adr_vit(gl->next->perv, gl->next, i, 1);
							return true;
							}
						else
							{
							posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
							return true;
							}
						}
					}

				if (gl->next->next->next->perv == NULL) //  кидает самую маленькую карту если первый ход
					{
					gl->next->next->next->perv = adr(gl->next->perv,gl->next, kol_vo(gl->next->perv,0), 1);
					return true;
					}								
	}
	
	return false;
}


bool comp_otbiv (gol *&gl) // компьютер отбивается
{
	sort(gl->next);

	if (gl->perv != NULL) // если колода непустая
	{
		for (int i = kol_vo(gl->next->perv,0); i > 0; i--) // закрываем наименьшей картой
			if ( ((adr_nonull(gl->next->perv, i, 1)->dost <15) && (adr_nonull(gl->next->perv, i, 1)->mast == posl(gl->next->next->next->perv)->mast) && (adr_nonull(gl->next->perv, i, 1)->dost > posl(gl->next->next->next->perv)->dost) ) || ( (adr_nonull(gl->next->perv, i, 1)->dost > 15) && (adr_nonull(gl->next->perv, i, 1)->dost < 21) && (adr_nonull(gl->next->perv, i, 1)->dost > posl(gl->next->next->next->perv)->dost) ) )
			{							
				posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
				return true;				
			}
		return false;
	}
	else // колода пуста
	{		
		
		bool dos = false;
		for (int i = kol_vo(gl->next->perv,0); i > 0; i--)		// поиск карты такого достоинства которого нет у игрока			
			if ( (adr_nonull(gl->next->perv, i, 1)->dost <15) && (adr_nonull(gl->next->perv, i, 1)->dost > posl(gl->next->next->next->perv)->dost) && (adr_nonull(gl->next->perv, i, 1)->mast == posl(gl->next->next->next->perv)->mast))
			{
				for (int j = kol_vo(gl->next->next->perv,0); j > 0; j--)
				{					
					if (adr_nonull(gl->next->perv, i, 1)->dost == adr_nonull(gl->next->next->perv, j, 1)->dost)
					{
						dos = true;
						break;
					}
				}

				if (dos == false)
				{							
					posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
					return true;				
				}
			
				dos = false;
			}	
			
		
		for (int i = kol_vo(gl->next->perv,0); i > 0; i--) // закрываем любой картой
			if ( ((adr_nonull(gl->next->perv, i, 1)->dost <15) && (adr_nonull(gl->next->perv, i, 1)->mast == posl(gl->next->next->next->perv)->mast) && (adr_nonull(gl->next->perv, i, 1)->dost > posl(gl->next->next->next->perv)->dost)) || ( (adr_nonull(gl->next->perv, i, 1)->dost > 15) && (adr_nonull(gl->next->perv, i, 1)->dost < 25) && (adr_nonull(gl->next->perv, i, 1)->dost > posl(gl->next->next->next->perv)->dost)) )
			{							
				posl(gl->next->next->next->perv)->next = adr_vit(gl->next->perv, gl->next, i, 1);
				return true;				
			}
		
		return false;
	}
}


void gr_comp(kart *&kk, RenderWindow &wd, int pix, int pixpl)
{
	Texture text1;
	text1.loadFromFile("rub.jpg");

	Sprite sp1;
	sp1.setTexture(text1);

	sp1.setPosition(pix,45);
	wd.draw(sp1);

	if (kk->next != NULL)
		gr_comp(kk->next,wd,pix+pixpl,pixpl);
}


void gr_hum(kart *&kk, RenderWindow &wd, int pix, int pixpl)
{
	kk->sp.setPosition(pix,540);
	kk->sp.setRotation(0);
	wd.draw(kk->sp);
	if (kk->next !=NULL)
		gr_hum(kk->next,wd,pix+pixpl,pixpl);
};

void com_hod_gr(kart *&kk, RenderWindow &wd, int px, int pxpl, bool per,int dk,int s4);

void hum_hod_gr(kart *&kk, RenderWindow &wd, int px, int pxpl, bool per,int dk, int s4)
{
	if (per == true)
	{
		px += pxpl;
		per = false;
	}
	else per = true;

	kk->sp.setPosition(px,315);
	kk->sp.setRotation(25);
	wd.draw(kk->sp);
	if (kk->next !=NULL)
		if (s4>dk)
			hum_hod_gr(kk->next,wd,px,pxpl,true,dk,s4+1);
		else com_hod_gr(kk->next,wd,px,pxpl,per,dk,s4+1);
}

void com_hod_gr(kart *&kk, RenderWindow &wd, int px, int pxpl, bool per,int dk, int s4)
{
	if (per == true)
	{
		px += pxpl;
		per = false;
	}
	else per = true;
	
	kk->sp.setPosition(px,270);
	kk->sp.setRotation(25);
	wd.draw(kk->sp);
	if (kk->next !=NULL)
		hum_hod_gr(kk->next,wd,px,pxpl,per,dk,s4+1);
}


void grafika(gol *&gl, RenderWindow &wd, bool player, int dk)
{
	if (gl->next->perv != NULL)
		gr_comp(gl->next->perv, wd, (980/(kol_vo(gl->next->perv,0)+1))-35, 980/(kol_vo(gl->next->perv,0)+1));
	if (gl->next->next->perv != NULL)
		gr_hum(gl->next->next->perv, wd, (980/(kol_vo(gl->next->next->perv,0)+1))-35, 980/(kol_vo(gl->next->next->perv,0)+1));
	
	if (gl->next->next->next->perv != NULL)
	{
		int kl = kol_vo(gl->next->next->next->perv,0);
		if (kl == 1) kl += 1;
		if (player == true)
			hum_hod_gr(gl->next->next->next->perv, wd, (980/(kl/2+1))-50-150, 150, true,dk,1);
		else com_hod_gr(gl->next->next->next->perv, wd, (980/(kl/2+1))-50-150, 150, true,dk,1);

	}
	
} 


bool hum_hod (gol *&gl, kart *& kk, int xx, int yy, int nom)
{
	int tmp = nom + 1;
	if (kk->sp.getPosition().x < xx && kk->sp.getPosition().x+71 > xx && kk->sp.getPosition().y < yy && kk->sp.getPosition().y+96 > yy)
	{
		if (kk->next != NULL)
		{   
			if (xx < kk->next->sp.getPosition().x)
			if (check (gl->next->next->next->perv, kk->dost) == true)
			{
				if (gl->next->next->next->perv == NULL)
				{
					gl->next->next->next->perv = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
					return true;
				}
				else
				{
					posl(gl->next->next->next->perv)->next = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
					return true;
				}
			}
		}
		else
			if (check (gl->next->next->next->perv, kk->dost) == true)
			{
				if (gl->next->next->next->perv == NULL)
				{
					gl->next->next->next->perv = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
					return true;
				}
				else
				{
					posl(gl->next->next->next->perv)->next = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
					return true;
				}
			}
	}
	if (kk->next != NULL)
		hum_hod (gl, kk->next, xx, yy, tmp);
	else return false;
}

bool hum_otbiv (gol *&gl, kart *& kk, int xx, int yy, int nom)
{
	int tmp = nom + 1;
	//cout<<kk->mast<<kk->dost<<" - "<<kk->sp.getPosition().x<<" < "<<xx<<" && "<<kk->sp.getPosition().x+kk->tx.getSize().x<<" > "<<xx<<" && "<<kk->sp.getPosition().y<<" < "<<yy<<" && "<<kk->sp.getPosition().y+kk->tx.getSize().y<<" > "<<yy<<" && "<<xx<<" < "<<kk->next->sp.getPosition().x<<endl;
	if (kk->sp.getPosition().x < xx && kk->sp.getPosition().x+71 > xx && kk->sp.getPosition().y < yy && kk->sp.getPosition().y+96 > yy)
	{
		if (kk->next != NULL)
		{   
			if (xx < kk->next->sp.getPosition().x)						
				if ((posl(gl->next->next->next->perv)->mast == kk->mast && posl(gl->next->next->next->perv)->dost < kk->dost) || (posl(gl->next->next->next->perv)->dost < kk->dost && kk->dost>15))
				{
					if (gl->next->next->next->perv == NULL)
					{
						gl->next->next->next->perv = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
						return true;
					}
					else
					{
						posl(gl->next->next->next->perv)->next = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
						return true;
					}
				}			
		}
		else
			if ((posl(gl->next->next->next->perv)->mast == kk->mast && posl(gl->next->next->next->perv)->dost < kk->dost) || (posl(gl->next->next->next->perv)->dost < kk->dost && kk->dost>15))
			{
				if (gl->next->next->next->perv == NULL)
				{
					gl->next->next->next->perv = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
					return true;
				}
				else
				{
					posl(gl->next->next->next->perv)->next = adr_vit(gl->next->next->perv, gl->next->next, nom+1, 1);
					return true;
				}
			}
			
	}
	if (kk->next != NULL)
		hum_otbiv (gl, kk->next, xx, yy, tmp);
	else return false;
}


int main()
{	
	new_game:
	srand(time(NULL));

	bool t = false;
	
	gol *hh;
	hh = new gol;
	hh = golova();

	cout<<hh->info<<endl;
	print(hh->perv,1);

	cout<<hh->next->info<<endl;
	print(hh->next->perv,1);

	cout<<hh->next->next->info<<endl;
	print(hh->next->next->perv,1);
	
	RenderWindow window(VideoMode(1200, 680), "Game \"DURAK\"", sf::Style::Titlebar | sf::Style::Close);

	Texture t_fon;
	t_fon.loadFromFile("fon.jpg");
	Sprite s_fon;
	s_fon.setTexture(t_fon);

	Texture t_rub;
	t_rub.loadFromFile("rub.jpg");
	Sprite s_rub;
	s_rub.setTexture(t_rub);
	s_rub.setPosition(1133,270);
	s_rub.setRotation(90);
	
	string slov = hh->perv->mast;
	slov += to_string(hh->perv->dost-10);
	slov += ".bmp";	
	
	Texture t_koz;
	t_koz.loadFromFile(slov);
	Sprite s_koz;
	s_koz.setTexture(t_koz);
	s_koz.setPosition(1050,295);
	

	Texture t_but;
	t_but.loadFromFile("button.gif");
	Sprite s_but;
	s_but.setTexture(t_but);
	s_but.setTextureRect(IntRect(4,2,70,28));
	s_but.setPosition(1025,565);

	Texture mar;
	mar.loadFromFile("rrect.gif");
	Sprite mar_s;
	mar_s.setTexture(mar);
	mar_s.setPosition(1000,100);

	Texture say_t;
	say_t.loadFromFile("say.gif");
	Sprite say_s;
	say_s.setTexture(say_t);
	say_s.setTextureRect(IntRect(0,0,210,75));
	say_s.setPosition(990,20);

	SoundBuffer buffer1;
	buffer1.loadFromFile("zv1.ogg");
	Sound zv1(buffer1);
	zv1.play();

	SoundBuffer buffer;
	buffer.loadFromFile("zv.ogg");
	Sound zv(buffer);
	

	float frame = 0;	

	bool player_hodit = true;
	bool hum_podk = false;
	bool player_otbilsya = true;
	int kol = 0;
	bool hod_hum = false;
	int k_sop = 5;
	int dok = 100;


	while (window.isOpen())
	{
		window.clear();
		window.draw(s_fon);
		window.draw(s_but);

		if (hh->perv != NULL)
		{
			window.draw(s_koz);
			if (hh->perv->next != NULL)
				window.draw(s_rub);	
		}
		
		if (player_hodit == true)
			grafika(hh, window, true,dok);
		else grafika(hh, window, false,dok);
		
		
		
		if (player_hodit == false)
		if (kol<k_sop)
			if (player_otbilsya == true)
				if (hh->next->perv != NULL && comp_hod(hh) == true)
				{
					kol += 1;
					player_otbilsya = false;
					zv.play();								
				}
				else
				{
					if (hh->next->perv == NULL && hh->perv == NULL && hh->next->next->perv == NULL)
						goto win;
					if (hh->next->perv == NULL && hh->perv == NULL)
						goto comp_win;
					if (hh->next->next->perv == NULL && hh->perv == NULL)
						goto hum_win;
					
					player_hodit = true;
					player_otbilsya = true;
					kol = 0;					
					hh->next->next->next->perv = NULL;
					popoln_c (hh);
					popoln_h (hh);
					sort(hh->next->next);
					if (kol_vo(hh->next->perv,0)<7)
						k_sop = kol_vo(hh->next->perv,0);
					else k_sop = 6;
				}
		
		Event event;
		while (window.pollEvent(event))
		{
			switch(event.type)
			{
				case Event::Closed:
					window.close();
					return 0;
					break;

				case Event::MouseButtonReleased:
					if(event.mouseButton.button == Mouse::Left)
						s_but.setTextureRect(IntRect(4,2,70,28));
						break;					
					
					
				case Event::MouseMoved:
					cout<<"x="<<event.mouseMove.x<<"  "<<"y="<<event.mouseMove.y<<endl;
					if (s_but.getPosition().x < event.mouseMove.x && s_but.getPosition().x+70 > event.mouseMove.x && s_but.getPosition().y < event.mouseMove.y && s_but.getPosition().y+28 > event.mouseMove.y)
						s_but.setTextureRect(IntRect(95,2,70,28));
					else s_but.setTextureRect(IntRect(4,2,70,28));
					break;

				case Event::MouseButtonPressed:
					if(event.mouseButton.button == Mouse::Left)						
					{
						cout<<"PRESS"<<"  "<<event.mouseButton.x<<"  "<<event.mouseButton.y<<endl;
						if (s_but.getPosition().x < event.mouseButton.x && s_but.getPosition().x+t_but.getSize().x > event.mouseButton.x && s_but.getPosition().y < event.mouseButton.y && s_but.getPosition().y+t_but.getSize().y > event.mouseButton.y)
						{
							cout<<"Pressed BUTTON"<<endl;
							cout<<hh->next->next->next->info<<endl;
							print(hh->next->next->next->perv,1);
							cout<<hh->next->next->info<<endl;
							print(hh->next->next->perv,1);
						}		
						
						if (player_hodit == true)
						{							
							if (kol <k_sop && hh->next->next->perv != NULL)
								if (hum_hod (hh, hh->next->next->perv, event.mouseButton.x, event.mouseButton.y, 0) == true)
								{
									zv.play();
									kol += 1;
									if (hum_podk == false)
										if (comp_otbiv (hh) == false)
										{
											hum_podk = true;
											dok = (kol * 2)-2;
										}
								}

							if (s_but.getPosition().x < event.mouseButton.x && s_but.getPosition().x+70 > event.mouseButton.x && s_but.getPosition().y < event.mouseButton.y && s_but.getPosition().y+28 > event.mouseButton.y && hh->next->next->next->perv != NULL)
							{
								s_but.setTextureRect(IntRect(187,2,70,28));
								if (hum_podk == false)
								{
									hh->next->next->next->perv = NULL;
									player_hodit = false;
								}
								else
								{
									posl(hh->next->perv)->next = hh->next->next->next->perv;
									hh->next->next->next->perv = NULL;
									
								}
								
								if (hh->next->perv == NULL && hh->perv == NULL && hh->next->next->perv == NULL)
									goto win;
								if (hh->next->perv == NULL && hh->perv == NULL)
									goto comp_win;
								if (hh->next->next->perv == NULL && hh->perv == NULL)
									goto hum_win;
								
								
								popoln_h (hh);
								popoln_c (hh);								
								
								sort(hh->next->next);
								kol = 0;
								if (kol_vo(hh->next->next->perv,0)<7)
									k_sop = kol_vo(hh->next->next->perv,0);
								else k_sop = 6;
								hum_podk = false;
								dok = 100;													
							}
						}
						else
						{
							if (player_otbilsya == false)							
								if (hum_otbiv (hh, hh->next->next->perv, event.mouseButton.x, event.mouseButton.y, 0) == true)
								{
									player_otbilsya = true;
									zv.play();
								}

							if (s_but.getPosition().x < event.mouseButton.x && s_but.getPosition().x+70 > event.mouseButton.x && s_but.getPosition().y < event.mouseButton.y && s_but.getPosition().y+28 > event.mouseButton.y && hh->next->next->next->perv != NULL)
							{
								s_but.setTextureRect(IntRect(187,2,70,28));
								bool tmp = true;
								if (hh->next->perv != NULL)
									while (tmp == true && kol<k_sop)
									{
										tmp = comp_hod(hh);
										kol += 1;
											
									}								
								
								if (hh->next->perv == NULL && hh->perv == NULL && hh->next->next->perv == NULL)
									goto win;
								if (hh->next->perv == NULL && hh->perv == NULL)
									goto comp_win;
								if (hh->next->next->perv == NULL && hh->perv == NULL)
									goto hum_win;
								
								
								posl(hh->next->next->perv)->next = 	hh->next->next->next->perv;
								hh->next->next->next->perv = NULL;
								player_otbilsya = true;
								kol = 0;
								popoln_c (hh);
								popoln_h (hh);
								sort(hh->next->next);
								if (kol_vo(hh->next->next->perv,0)<7)
									k_sop = kol_vo(hh->next->perv,0);
								else k_sop = 6;								
							}

						}						
							
						
						cout<<hh->next->info<<endl;
						print(hh->next->perv,1);				

						
					}
					break;

				

			}
		}	
		
		if (player_hodit == true && hum_podk != true)
			say_s.setTextureRect(IntRect(0,0,205,75));
		if (player_hodit == true && hum_podk == true)
			say_s.setTextureRect(IntRect(0,77,205,75));
		if (player_hodit == false)
			say_s.setTextureRect(IntRect(0,150,205,75));

		
		
		if (frame <3)
			mar_s.move(0.3,0);
		else mar_s.move(-0.3,0);
		frame += 0.012;
		if (frame > 6) frame -= 6;
		mar_s.setTextureRect(IntRect(40*int(frame),0,40,90));
		
		window.draw(mar_s);		
		window.draw(say_s);

		window.display();
	}


	
	hum_win:
	{
	window.close();
	
	RenderWindow window(VideoMode(770, 446), "Game \"DURAK\"", sf::Style::Titlebar | sf::Style::Close);
	
	Texture t_win;
	t_win.loadFromFile("win.jpg");
	Sprite s_win;
	s_win.setTexture(t_win);

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			switch(event.type)
			{
				case Event::Closed:
					window.close();
					break;

				case Event::MouseButtonPressed:
					if(event.mouseButton.button == Mouse::Left)
						goto new_game;
					break;
			}
		}
		window.clear();
		window.draw(s_win);
		window.display();
	}

	t = true;
	}
	
	comp_win: 
	{
	if (t == false) 
	{
		window.close();
		RenderWindow window(VideoMode(770, 446), "Game \"DURAK\"", sf::Style::Titlebar | sf::Style::Close);

		Texture t_win;
		t_win.loadFromFile("lose.jpg");
		Sprite s_win;
		s_win.setTexture(t_win);

		while (window.isOpen())
		{
			

			Event event;
			while (window.pollEvent(event))
			{
				switch(event.type)
				{
					case Event::Closed:
						window.close();
						break;

					case Event::MouseButtonPressed:
						if(event.mouseButton.button == Mouse::Left)
							goto new_game;
						break;
				}
			}
			window.clear();
			window.draw(s_win);
			window.display();
		}
		t = true;
	}
	}
	win: 
	{
	if (t == false)
	{
		window.close();
		RenderWindow win(VideoMode(770, 446), "Game \"DURAK\"", sf::Style::Titlebar | sf::Style::Close);

		Texture t_win;
		t_win.loadFromFile("nic.jpg");
		Sprite s_win;
		s_win.setTexture(t_win);

		while (win.isOpen())
		{
			win.clear();
			win.draw(s_win);

			Event event;
			while (win.pollEvent(event))
			{
				switch(event.type)
				{
					case Event::Closed:
						win.close();
						break;

					case Event::MouseButtonPressed:
						if(event.mouseButton.button == Mouse::Left)
							goto new_game;
						break;
				}
			}
			win.display();
		}
	}
	}
	
	//system("pause");	
	return 0;
}
