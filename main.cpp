#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <map>
#include <unordered_map>
#include <sstream>

#include <windows.h>

//#include <json.hpp>
//sing json = nlohmann::json;

using namespace std;

namespace Console {
	class IConcole
	{
	public:
		virtual ~IConcole()
		{
			::SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	protected:
		static const HANDLE hStdOut;
	};
	const HANDLE IConcole::hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);

	class Pink : public IConcole
	{
	public:
		Pink()
		{
			::SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
		}
	};

	class Golden : public IConcole
	{
	public:
		Golden()
		{
			::SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN);
		}
	};

	class Blue : public IConcole
	{
	public:
		Blue()
		{
			::SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		}
	};
}

namespace Genshin {
	enum class Rank
	{
		ssr = 0x5, sr = 0x4, r = 0x3
	};

	class IPool
	{
	public:
		using ret_ty = pair<int, string>;
		virtual ret_ty giveMeOne(const Rank&) = 0;
	};

	class Common : public IPool
	{
	public:
		ret_ty giveMeOne(const Rank& rank)
		{
			static const vector<string> char5 = {
				"迪卢克", "琴", "莫娜", "七七", "刻晴"
			};
			static const vector<string> char4 = {
				"芭芭拉", "班尼特", "迪奥娜", "菲谢尔", "丽莎", "雷泽", "诺艾尔", "凯亚", "安柏", "砂糖", "凝光", "重云", "北斗", "香菱", "行秋", "辛焱", "烟绯", "罗莎莉亚", "早柚"
			};

			mt19937_64 gen(random_device{ }());
			if (rank == Rank::ssr)
			{
				uniform_int_distribution<> dist(0, static_cast<int>(char5.size() - 1));
				return { true, char5[dist(gen)] };
			}
			else if (rank == Rank::sr)
			{
				uniform_int_distribution<> dist(0, static_cast<int>(char4.size() - 1));
				return { true, char4[dist(gen)] };
			}
			else if (rank == Rank::r)
			{
				return { true, "三星" };
			}
			return { false, "" };
		}
	};

	class LimitA : public IPool
	{
	public:
		ret_ty giveMeOne(const Rank& rank)
		{
			static const vector<string> char5 = {
				"温迪", "可莉", "达达利亚", "钟离", "阿贝多", "甘雨", "魈", "胡桃", "优菈", "万叶", "神里绫华", "霄宫", "雷电将军", "珊瑚宫心海"
			};
			static const string char5up = char5[char5.size() - 2];
			static const vector<string> char4{
				"辛焱", "烟绯", "罗莎莉亚", "早柚"
			};

			mt19937_64 gen(random_device{ }());
			if (rank == Rank::ssr)
			{
				uniform_int_distribution<> dist(0, 1);
				if (Nb || dist(gen))
				{
					Nb = false;
					return pair<int, string>{ 2, char5up };
				}
				else
				{
					Nb = true;
					return Common{ }.giveMeOne(rank);
				}
			}
			else if (rank == Rank::sr)
			{
				uniform_int_distribution<> dist(0, 1);
				uniform_int_distribution<> dist2(0, 3);
				return dist(gen) ? Common{ }.giveMeOne(rank) : pair<int, string>(2, char4[dist2(gen)]);
			}
			else if (rank == Rank::r)
			{
				return { true, "三星" };
			}
			return { false, "" };
		}
	private:
		bool Nb = false;
	};

	class LimitB : public IPool
	{
	public:
		ret_ty giveMeOne(const Rank& rank)
		{
			return { false, "" };
		}
	};

	enum class Pool
	{
		奔行世间, 限定角色, 限定武器
	};

	class Gacha
	{
	public:
		Gacha() = delete;
		Gacha(const Gacha&) = delete;
		Gacha(Gacha&&) = delete;
		const Gacha& operator=(const Gacha&) = delete;

		Gacha(const Pool& plt) :
			ths(nullptr)
		{
			if (plt == Pool::奔行世间)
			{
				ths = new Common();
			}
			else if (plt == Pool::限定角色)
			{
				ths = new LimitA();
			}
			else if (plt == Pool::限定武器)
			{
				ths = new LimitB();
			}
			memset(&data, 0, sizeof(data));
			memset(&guarantee, 0, sizeof(guarantee));
			guarantee.ssr = 1;
		}
		~Gacha()
		{
			delete ths;
		}

		pair<string, Rank> roll()
		{
			mt19937_64 gen(random_device{ }());
			uniform_int_distribution<> dist(0, 999);

			while (data.fate <= 0) data.getFate(1);
			data.fate--;
			guarantee.cnt++;

			auto rnk = getRank();
			if (auto rslt = ths->giveMeOne(rnk); rslt.first)
			{
				history[guarantee.cnt] = { guarantee.ssr, rslt.second, rnk };
				if (rnk == Rank::ssr) guarantee.ssr = 0;
				guarantee.ssr++;
				return { rslt.second, rnk };
			}
			throw exception("出了亿点小问题");
		}

		void analyse()
		{
			int cnt3 = 0, cnt4 = 0, cnt5 = 0, cnt = 0;
			stringstream oss;
			unordered_map<string, int> char5s;

			for (auto& e : history)
			{
				switch (e.second.rank)
				{
					case Rank::ssr:
					{
						cnt5++;
						oss << e.second.name << '[' << e.second.ssr << ']' << endl;
						if (char5s.count(e.second.name)) char5s[e.second.name]++;
						else
						{
							char5s[e.second.name] = 1;
						}
					}
					break;
					case Rank::sr:
					{
						cnt4++;
					}
					break;
					case Rank::r:
					{
						cnt3++;
					}
					break;
					default:
						cnt++;
				}
				//printf_s("%d\t名字：%s\t 保底内%d\n", e.first, e.second.name.c_str(), e.second.ssr);
			}

			cout << "共获得三星" << cnt3 << "个" << endl
				<< "共获得四星" << cnt4 << "个" << endl
				<< "共获得五星" << cnt5 << "个" << endl << endl
				<< oss.str() << endl
				<< cnt << " errors." << endl
				<< endl;

			vector<pair<string, int>> c5(char5s.begin(), char5s.end());
			sort(c5.begin(), c5.end(), [] (const pair<string, int>& l, const pair<string, int>& r) {
				return l.second > r.second;
				});
			for (auto& cp : c5)
			{
				cout << cp.first << "\t" << cp.second << endl;
			}
			cout << endl;

			cout << "共剩余：\n"
				<< "\t球" << data.fate << endl
				<< "\t原石：" << data.stone << endl
				<< "\t创世结晶：" << data.crystal << endl
				<< "\t消耗rmb：" << data.cost << endl
				<< endl;
		}
	private:
		Rank getRank()
		{
			std::uniform_int_distribution<> dist(0, 999);
			std::mt19937 gen(std::random_device{ }());
			int dg = dist(gen);

			if ((dg -= (6 + (guarantee.ssr > 70 ? guarantee.ssr - 70 : 0) * 50)) < 0)
				return Rank::ssr;

			if ((dg -= (51 + (guarantee.sr > 5 ? guarantee.sr - 5 : 0) * 250)) < 0)
			{
				guarantee.sr = 0;
				return Rank::sr;
			}
			else
			{
				guarantee.sr++;
			}

			return Rank::r;
		}
	private:
		IPool* ths;

		using hisdata = struct
		{
			int ssr;		//保底内第n抽
			string name;
			Rank rank;
		};
		map<int, hisdata> history;

		struct
		{
			int ssr;
			int sr;
			int cnt;
		} guarantee;
		struct
		{
			unsigned fate;
			unsigned stone;			// 原石
			unsigned crystal;		// 结晶
			unsigned cost;			// 钱
			unsigned shining;		// 星辉

			void getFate(unsigned n)
			{
				while (shining > 5 && n >= 0)
				{
					shining -= 5;
					fate++;
					n--;
				}
				while (160 * n > stone)
				{
					getStone(160);
				}
				stone -= 160 * n;
				fate += n;
			}
			void getStone(unsigned n)
			{
				while (crystal < n)
				{
					getCrystal();
				}
				crystal -= n;
				stone += n;
			}
			void getCrystal()
			{
				cost += 648;
				crystal += 8088;
			}
		} data;
	};
}

// https://api-takumi.mihoyo.com/binding/api/getUserGameRolesByCookie?game_biz=hk4e_cn
int main()
{
	auto cmn = make_unique<Genshin::Gacha>(Genshin::Pool::限定角色);

	try
	{
		for (int i = 0; i < 45; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				unique_ptr<Console::IConcole> _;
				auto rslt = cmn->roll();
				switch (rslt.second)
				{
					case Genshin::Rank::ssr:
						_ = make_unique<Console::Golden>();
						break;
					case Genshin::Rank::sr:
						_ = make_unique<Console::Pink>();
						break;
					case Genshin::Rank::r:
						_ = make_unique<Console::Blue>();
						break;
				}
				cout << rslt.first << " ";
			}
			cout << endl;
		}

		cmn->analyse();
	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
	}

	return 0;
}