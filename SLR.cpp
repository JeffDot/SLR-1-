#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <stack>
#include <algorithm>
#include <assert.h>

using namespace std;

enum DOWHAT {MOVIN ,RANK ,ERR ,ACC};
//变化栈
typedef struct stackElem
{
	int order;
	stack<int> state_stack;
	stack<char> symbol_stack;
	string in_str;
	pair<DOWHAT ,int> action;
}sE;
typedef vector<sE> sEArray;//存储状态栈变化过程
typedef vector<char> cary;//表示终结符号和非终结符号的集合
typedef vector<string> cndt;//候选项的集合
typedef map<char ,cndt> gramap;//用于求FIRST和FOLLOW集合所用的表示文法的数据结构
typedef map<char ,cary> FS;//表示FOLLOW和FIRST集合的数据结构
typedef vector<pair<int ,string>> Gtype;//文法的数据结构
typedef pair<int ,int> Itm;//项的数据结构,<,>分别表示是增广文法的第几个产生式和点的位置
typedef vector<pair<int ,int>> ItmAry;//表示项集的数据结构
typedef vector<pair<int ,ItmAry>> ItmAryRace;//表示项集族的数据结构
typedef map<pair<int ,char> ,pair<DOWHAT ,int>> How2DoList;//ACTION的数据结构
typedef map<pair<int ,char> ,int> GOtype;//GOTO表的数据结构

string int2string(int num)
{
	string out = "";
	bool is_stop = true;
	while(is_stop)
	{
		if(num / 10 == 0)
		{
			is_stop = false;			
		}		
		string tmp;
		tmp = (num % 10) + '0';
		tmp += out;
		out = tmp;
		num = num/10;
	}
	return out;
}
void GetG(Gtype &G ,gramap &Gmap ,cary &nFnl ,cary &Fnl)
{
	ifstream in;
	in.open("Gin.txt");	
	if(!in.is_open())
	{
		cerr << "Can't Open File" << " Gin.txt " << endl;
		exit(0);
	}	
	string inG = "";
	int i = 0;			
	while(getline(in ,inG ,'\n') && inG != "")
	{
		pair<int ,string> tmp;
		tmp.first = i;
		tmp.second = inG;
		G.push_back(tmp);
		i ++;
	}
	in.clear();
	in.seekg(0);
	while(getline(in ,inG ,'\n') && inG != "")
	{
		nFnl.push_back(inG[0]);
		Gmap[inG[0]].push_back(inG.substr(3,inG.size() - 1));		
	}
	//重置文件指针到文件头
	in.clear();
	in.seekg(0);
	while(getline(in ,inG ,'\n') && inG != "")
	{
		char ch;
		for(string::size_type i = 3;i != inG.size();i ++)
		{
			ch = inG[i];
			if(find(nFnl.begin() ,nFnl.end() ,ch) == nFnl.end())
				Fnl.push_back(ch);
		}			
	}
	cary::iterator inFnlor = unique(nFnl.begin() ,nFnl.end());
	nFnl.erase(inFnlor ,nFnl.end());
	cary::iterator iFnlor = unique(Fnl.begin() ,Fnl.end());
	Fnl.erase(iFnlor ,Fnl.end());
	in.close();	
}
char First(char a ,FS &fst ,gramap &gmap ,cary nf)
{
	if(fst[a].empty())
	{
		cndt::iterator igm = gmap[a].begin();		
		while(igm != gmap[a].end())//对所有非终结符
		{			
			bool IsInclude = (find(nf.begin() ,nf.end() ,(*igm)[0]) == nf.end());//是否是终结符
			if(IsInclude)//对形如U－a…的产生式（其中a是终结符），把a收入到First(U)中
				(fst[a]).push_back((*igm)[0]);					
			else
			{
				if(((*igm)[0] != a))
				{
					char tmp = First((*igm)[0] ,fst ,gmap ,nf );
					cary::iterator ic = fst[tmp].begin();
					while(ic != fst[tmp].end())
					{
						fst[a].push_back(*ic);
						ic ++;
					}				
				}				
			}	
			igm ++;
		}
	}
	return a;
}
void Follow(char a ,FS &fol ,gramap &gmap ,cary nf ,cary f ,FS &fst)
{
	//若为开始符号则将￥符	号加入到follow(S)中去
	if(a == 'S')
	{
		fol[a].push_back('$');
	}
	//对文法中所有的产生式
	cary::iterator infnlor = nf.begin();
	while(infnlor != nf.end())
	{	
		cndt::iterator icy = gmap[(*infnlor)].begin();
		//对非终结符的每个产生式
		while(icy != gmap[(*infnlor)].end())
		{
			string::size_type i = 0;
			char ch;
			while(1)
			{
				if(i == (*icy).size())
					break;					
				if((ch = (*icy)[i]) == a)//==优先级比=高
					break;
				i ++;				
			}
			if(i == (*icy).size())
			{
				icy ++;
				break;
			}
			else if(i == (*icy).size() - 1)
			{
				if(!fol[(*infnlor)].empty())
				{
					cary::iterator itmp = fol[(*infnlor)].begin();
					while(itmp != fol[(*infnlor)].end())
					{
						if(find(fol[a].begin() ,fol[a].end() ,*itmp) == fol[a].end())
							fol[a].push_back(*itmp);
						itmp ++;
					}
				}
				else//当follow(P)为空
				{
					Follow((*infnlor) ,fol ,gmap ,nf ,f ,fst);
					cary::iterator itmp = fol[(*infnlor)].begin();
					while(itmp != fol[(*infnlor)].end())
					{
						if(find(fol[a].begin() ,fol[a].end() ,*itmp) == fol[a].end())
							fol[a].push_back(*itmp);
						itmp ++;
					}
				}
			}
			else
			{
				ch = (*icy)[i + 1];
				if(find(f.begin() ,f.end() ,ch) != f.end() && find(fol[a].begin() ,fol[a].end() ,ch) == fol[a].end())
					fol[a].push_back(ch);
				//如果U后跟着的是非终结符P,将First(P)加入到FOLLOW(U)中
				if((find(nf.begin() ,nf.end() ,ch) != nf.end()))
				{
					cary::iterator itmp = fst[ch].begin();		
					while(itmp != fst[ch].end())
					{
						//判断fol集合中是否已经含有该项
						if((find(fol[a].begin() ,fol[a].end() ,*itmp) == fol[a].end())?1 :0)
							fol[a].push_back(*itmp);
						itmp ++;
					}
				}//如果U后跟着的是非终结符P,将First(P)加入到FOLLOW(U)中
				if((find(nf.begin() ,nf.end() ,ch) == nf.end()))
				{
					cary::iterator itmp = fst[ch].begin();		
					while(itmp != fst[ch].end())
					{
						//判断fol集合中是否已经含有该项
						if((find(fol[a].begin() ,fol[a].end() ,*itmp) == fol[a].end())?1 :0)
							fol[a].push_back(*itmp);
						itmp ++;
					}
				}
			}
			icy ++;
		}
		infnlor ++;
	}
}
//求闭包
ItmAry Closure(ItmAry &I ,Gtype gv ,cary f ,cary nf)
{
	ItmAry::size_type i;
	for(i = 0;i != I.size();i ++)
	{
		char ch;
		if(ItmAry::size_type((3 + I[i].second)) < gv[I[i].first].second.size())
			ch = gv[I[i].first].second[3 + I[i].second];
		else
			ch = NULL;
		if(ch != NULL || find(f.begin() ,f.end() ,ch) == f.end())		
		{
			Gtype::iterator itmp = gv.begin();
			while(itmp != gv.end())
			{
				if((*itmp).second[0] == ch)
				{
					Itm tmp((*itmp).first ,0);
					if(find(I.begin() ,I.end() ,tmp) == I.end())
						I.push_back(tmp);					
				}
				itmp ++;
			}
		}
	}
	return I;
}
//GOTO函数
ItmAry GOTO(ItmAry &I ,char X ,Gtype gv ,cary f ,cary nf)
{
	ItmAry Inew;
	for(ItmAry::size_type i = 0;i != I.size();i ++)
	{
		char ch;
		if(ItmAry::size_type((3 + I[i].second)) < gv[I[i].first].second.size())
			ch = gv[I[i].first].second[3 + I[i].second];
		else
			ch = NULL;
		if(ch == X)
		{
			Itm tmp(I[i].first ,I[i].second + 1);
			if(find(Inew.begin() ,Inew.end() ,tmp) == Inew.end())
				Inew.push_back(tmp);
		}
	}
	Closure(Inew ,gv ,f ,nf);
	return Inew;
}
//求规范项集族
ItmAryRace StndItmRc(Gtype gv ,cary f ,cary nf)
{
	Itm a(0 ,0);
	ItmAry start;
	start.push_back(a);
	ItmAry tmp0 = Closure(start ,gv ,f ,nf);
	ItmAryRace C;
	int count = 0;
	pair<int ,ItmAry> tmp1(count ,tmp0);
	count ++;
	C.push_back(tmp1);
	for(ItmAryRace::size_type i = 0;i != C.size();i ++)
	{
		ItmAry tmp2 = C[i].second;
		cary::iterator ifor = f.begin();
		while(ifor != f.end())
		{
			ItmAry tmp3 = GOTO(tmp2 ,(*ifor) ,gv ,f ,nf); 
			pair<int ,ItmAry> tmp4(count ,tmp3);			
			if(!tmp3.empty())
			{
				bool IsIn = true;
				ItmAryRace::iterator itmp = C.begin();
				while(itmp != C.end())
				{
					if((*itmp).second == tmp3)
						IsIn = false;
					itmp ++;
				}
				if(IsIn)
				{					
					C.push_back(tmp4);
					count ++;					
				}
			}
			ifor ++;
		}
		cary::iterator infor = nf.begin();
		while(infor != nf.end())
		{
			ItmAry tmp3 = GOTO(tmp2 ,(*infor) ,gv ,f ,nf); 
			pair<int ,ItmAry> tmp4(count ,tmp3);			
			if(!tmp3.empty())
			{
				bool IsIn = true;
				ItmAryRace::iterator itmp = C.begin();
				while(itmp != C.end())
				{
					if((*itmp).second == tmp3)
						IsIn = false;
					itmp ++;
				}
				if(IsIn)
				{
					C.push_back(tmp4);
					count ++;					
				}
			}
			infor ++;
		}
	}
	return C;
}
//构造SLR分析表
void SLR_LIST(How2DoList &ACTION ,GOtype &GO ,ItmAryRace C ,cary f ,cary nf ,Gtype gv ,FS fol)
{
	for(ItmAryRace::size_type i = 0;i <= C.size() - 1;i ++)
	{
		for(cary::size_type j = 0;j <= f.size() - 1;j ++)
		{			
			pair<int ,char> errtmp(i ,f[j]);
			pair<DOWHAT , int> err(ERR ,-1);
			ACTION[errtmp] = err;
		}
		for(cary::size_type j = 0;j <= nf.size() - 1;j ++)
		{
			pair<int ,char> errtmp(i ,nf[j]);			
			GO[errtmp] = -1;
		}
	}
	ItmAryRace::size_type i;
	for(i = 0;i != C.size();i ++)//对所有的状态(项集)
	{
		//[A->α.aβ]在Ii中且GOTO(Ii ,a) = Ij,将ACTION[i ,a]设置为MOVIN j
		ItmAry tmp;
		cary::iterator itmp_f = f.begin();
		while(itmp_f != f.end())
		{
			tmp = GOTO(C[i].second ,(*itmp_f) ,gv ,f ,nf);
			if(!tmp.empty())
			{
				ItmAryRace::size_type j;
				bool IsInC = false;
				for(j = 0;j != C.size();j ++)
				{
					if(C[j].second == tmp)
					{
						IsInC = true;
						break;
					}
				}
				if(IsInC)
				{
					pair<int ,char> tmp0(i ,(*itmp_f));
					pair<DOWHAT ,int> test1(ERR ,-1);
					if(ACTION[tmp0] == test1)
					{
						pair<DOWHAT ,int> tmp1(MOVIN ,j);
						ACTION[tmp0] = tmp1;
					}
				}
			}	
			itmp_f ++;

		}
		cary::iterator itmp_nf = nf.begin() + 1;
		//对于所有非终结符号,若GOTO[i ,A] = Ij,则GO[i ,A] = j
		while(itmp_nf != nf.end())
		{
			tmp = GOTO(C[i].second ,(*itmp_nf) ,gv ,f ,nf);
			ItmAryRace::size_type j;
			if(!tmp.empty())
			{
				bool IsInC = false;
				for(j = 0;j != C.size();j ++)
				{
					if(C[j].second == tmp)
					{
						IsInC = true;
						break;
					}
				}
				if(IsInC)
				{
					pair<int ,char> tmp0(i ,(*itmp_nf));
					GO[tmp0] = j;
				}
			}	
			itmp_nf ++;
		}
		//[A->α.]在Ii中,将FOLLOW(A)中所有的a,将ACTION[i ,a]设置为规约A->α		
		ItmAry::size_type j;
		for(j = 0;j != C[i].second.size();j ++)
		{
			Itm test = C[i].second[j];
			Gtype::iterator igmp = gv.begin();	
			while(igmp != gv.end())
			{			
				Itm tmpa((*igmp).first ,(*igmp).second.length() - 3);			
				if((test) == tmpa)
				{
					//cary::iterator itmp0 = fol[(*igmp).first].begin();
					cary::size_type icf = 0;
					while(icf != fol[(*igmp).second[0]].size())
					{
						/*if((*icf) == '+' && i == 2)
							cout << " ";*/
						char ch = fol[(*igmp).second[0]][icf];
						pair<int ,char> tmp0(i ,ch);
						pair<DOWHAT ,int> tmp1(RANK ,(*igmp).first);
						ACTION[tmp0] = tmp1;
						icf ++;
					}
				}
				igmp ++;
			}
			
		}
		//[S->S.]在Ii中将ACTION[i ,$]设置为ACC
		pair<int ,int> zero(0 ,1);
		ItmAry::iterator izero = C[i].second.begin();
		while(izero != C[i].second.end())
		{
			if(zero == (*izero))
			{
				pair<int ,char> tmp0(i ,'$');
				pair<DOWHAT ,int> tmp1(ACC ,-1);
				ACTION[tmp0] = tmp1;
			}
			izero ++;
		}
	}	
}
//分析过程
void LR(How2DoList &ACTION ,GOtype &GO ,string in_test ,Gtype G ,string &out)
{
	sEArray S;
	sEArray::size_type count = 0;
	sE s;
	pair<DOWHAT ,int> tmp0(MOVIN ,-1);
	s.state_stack.push(0);
	s.symbol_stack.push('/');
	s.in_str = in_test;
	s.action = tmp0;
	s.order = 0;
	char ch = s.in_str[0];
	while(1)
	{		
		//ch = s.symbol_stack.top();
		pair<int ,char> tmp(s.state_stack.top() ,ch);
		tmp0 = ACTION[tmp];
		s.action = tmp0;
		S.push_back(s);
		if(tmp0.first == MOVIN)
		{
			s.order ++;
			s.state_stack.push(tmp0.second);
			s.symbol_stack.push(ch);
			s.in_str = s.in_str.substr(1 ,s.in_str.size() - 1);
			ch = s.in_str[0];
		}
		else if(tmp0.first == RANK)
		{
			s.order ++;
			int n = G[tmp0.second].second.size() - 3;
			for(int i = 0;i <= n - 1;i ++)
			{
				s.state_stack.pop();
				s.symbol_stack.pop();
			}
			s.symbol_stack.push(G[tmp0.second].second[0]);
			pair<int ,char> tmp_x(s.state_stack.top() ,s.symbol_stack.top());
			int test = GO[tmp_x];
			s.state_stack.push(test);
		}
		else if(tmp0.first == ACC)
		{		
			break;
		}
		else
		{
			pair<DOWHAT ,int> err(ERR ,-1);
			s.action = err;
			S.push_back(s);			
			break;
		}
	}		
	sEArray::iterator iS = S.begin();
	while(iS != S.end())
	{
		out += int2string((*iS).order);
		if((*iS).order > 9)
			out += "    ";
		else
			out += "     ";		
		int countblank = 26;
		while(!(*iS).state_stack.empty())
		{
			out += int2string((*iS).state_stack.top());
			if((*iS).state_stack.top() > 9)
				out += " ";
			else
				out += "  ";
			(*iS).state_stack.pop();
			countblank -= 3;
		}
		for(int i = countblank - 1;i >= 0;i --)
		{
			out += " ";
		}
		countblank = 24;
		while(!(*iS).symbol_stack.empty())
		{
			out += (*iS).symbol_stack.top();
			out += " ";
			(*iS).symbol_stack.pop();
			countblank -= 2;
		}
		for(int i = countblank - 1;i >= 0;i --)
		{
			out += " ";
		}
		countblank = 23;
		out += (*iS).in_str;
		for(int i = countblank - (*iS).in_str.size();i >= 0;i --)
		{
			out += " ";
		}
		pair<DOWHAT ,int> tmp_out2 = (*iS).action;
		if(tmp_out2.first == MOVIN)
		{
			out += "移入";
			out += int2string(tmp_out2.second);
		}
		else if(tmp_out2.first == RANK)
		{
			out += "归约到产生式";
			out += int2string(tmp_out2.second);
		}
		else if(tmp_out2.first == ACC)
		{
			out += "完成";
		}
		else
		{
			out += "存在冲突";
		}
		out += "\n";
		iS ++;
	}
	//cout << out << endl;
}

int main()
{
	//非终结符集合
	cary nf;
	//终结符集合
	cary f;
	//求first和follow集合所用文法表示
	gramap gm;
	//文法表示
	Gtype G;
	//读入文件名称
	//first和follow集合
	FS fst;
	FS fol;
	//文法的项集族
	ItmAryRace C;
	//ACTION和GOTO表
	How2DoList ACTION;
	GOtype GO;
	//取得文法,终结符和非终结符
	GetG(G ,gm ,nf ,f);
	
	//求first和follow集合
	cary::iterator inf = nf.begin();
	while(inf != nf.end())
	{
		First(*inf ,fst ,gm ,nf);
		inf ++;
	}
	inf = nf.begin();
	while(inf != nf.end())
	{
		Follow(*inf ,fol ,gm ,nf ,f ,fst);
		inf ++;
	}
	
	f.push_back('$');
	//求项集族
	C = StndItmRc(G ,f ,nf);
	ofstream Coutfile;
	Coutfile.open("ItmArrayRace.txt");
	string Cout = "";
	for(ItmAryRace::size_type i = 0;i != C.size();i ++)
	{
		Cout += int2string(C[i].first);
		if(C[i].first > 9)
			Cout += " :";
		else
			Cout += "  :";
		for(ItmAry::size_type j = 0;j != C[i].second.size();j ++)
		{
			Cout += "<";
			Cout += int2string(C[i].second[j].first);
			if(C[i].second[j].first > 9)
				Cout += ",";
			else
				Cout += " ,";
			Cout += int2string(C[i].second[j].second);
			if(C[i].second[j].second > 9)
				Cout += ">";
			else
				Cout += " >";
			Cout += "   ";
		}
		Cout += "\n";
	}
	Coutfile << Cout;
	//构造SLR分析表
	SLR_LIST(ACTION ,GO ,C ,f ,nf ,G ,fol);
	//-----------------------------------------------------
	//将ACTION表和GO表发送到文件中去
	ofstream AGout;
	string outstr = "    ";
	AGout.open("ActionGoList.txt");
	cary::iterator i4outnf = f.begin();
	while(i4outnf != f.end())
	{
		outstr += (*i4outnf);
		outstr += "    ";
		i4outnf ++;
	}
	i4outnf = nf.begin() + 1;
	while(i4outnf != nf.end())
	{
		outstr += (*i4outnf);
		outstr += "    ";
		i4outnf ++;
	}
	outstr += "\n";
	for(int i = 0;i != C.size();i ++)
	{
		outstr += int2string(i);
		if(i > 9)		
			outstr += "  ";
		else
			outstr += "   ";
		cary::iterator if0 = f.begin();
		while(if0 != f.end())
		{
			pair<int ,char> tmp_outfile0(i ,(*if0));
			pair<DOWHAT ,int> tmp_outfile1 = ACTION[tmp_outfile0];
			if(tmp_outfile1.first == MOVIN)
			{
				outstr += 's';
				outstr += int2string(tmp_outfile1.second);
				if(tmp_outfile1.second > 9)
					outstr += "  ";
				else
					outstr += "   ";
			}
			else if(tmp_outfile1.first == RANK)
			{
				outstr += 'r';
				outstr += int2string(tmp_outfile1.second);
				if(tmp_outfile1.second > 9)
					outstr += "  ";
				else
					outstr += "   ";
			}
			else if(tmp_outfile1.first == ACC)
			{
				outstr += "acc  ";
			}
			else
			{
				outstr += "er   ";
			}
			if0 ++;
		}
		if0 = nf.begin() + 1;
		while(if0 != nf.end())
		{
			pair<int ,char> tmp_outfile0(i ,(*if0));
			int tmp_outfile1 = GO[tmp_outfile0];			
			if(tmp_outfile1 == -1)
			{
				outstr += "     ";
			}
			else
			{
				outstr += int2string(GO[tmp_outfile0]);
				if(GO[tmp_outfile0] > 9)
					outstr += "   ";
				else
					outstr += "    ";
			}
			if0 ++;
		}
		outstr += "\n";
	}
	AGout << outstr;    
	AGout.close();

	//SLR分析
	string test_in_name = "in_test.txt";
	fstream test_in;
	test_in.open(test_in_name ,ios::in);
	string result = "";
	if(!test_in.is_open())
	{
		result +=  "Can't Find";
		exit(0);
	}
	string in_test = "";
	string in_tmp = "";
	while(getline(test_in ,in_test ,'\n'))
		in_tmp += in_test;
	in_tmp += "$";
	string out = "序号  状态					    符号				        输入串				    动作		\n";
	LR(ACTION ,GO ,in_tmp ,G ,out);	
	ofstream resultfile;
	resultfile.open("Syntax_Result.txt");
	resultfile << out;
	resultfile.close();
	Coutfile.close();
	cout << "分析完成" << endl;
	cin.get();
	return 0;
}