#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <iostream>
#include <sstream>
#define GRIDSIZE 25
#define LVL1_WIDTH 9
#define LVL1_HEIGHT 9
#define LVL1_NUM 10
#define LVL2_WIDTH 16
#define LVL2_HEIGHT 16
#define LVL2_NUM 40
#define LVL3_WIDTH 30
#define LVL3_HEIGHT 16
#define LVL3_NUM 99
//枚举定义网格状态
enum GRIDSTATE
{
	ncNULL,				//空地
	ncUNDOWN,		    //背景方块
	ncMINE,				//地雷
	ncONE,				//数字1
	ncTWO,				//数字2
	ncTHREE,			//数字3
	ncFOUR,				//数字4
	ncFIVE,				//数字5
	ncSIX,				//数字6
	ncSEVEN,			//数字7
	ncEIGHT,			//数字8
	ncFLAG,				//标记
	ncQ,				//问号
	ncX,				//备用
	ncBOMBING,			//爆炸的雷
	ncUNFOUND			//未检测出来的雷
};
enum GAMEOVERSTATE
{
	ncNo,		//游戏未结束
	ncWIN,		//游戏胜利
	ncLOSE,		//游戏失败
};
using namespace sf;			//SFML中的每个类都位于该命名空间之下，不设定sf命名空间的话，相应的函数前需要用作用域解析符，例如 sf::VideoMode(width* GRIDSIZE, height* GRIDSIZE)

class MINE
{
public:
	int mState;			//栅格的状态
	int mStateBackUp;	//栅格原始状态备份
	bool isPress;		//栅格是否被点击
	bool isPressBackUp;
};
class Game
{
public:
	sf::RenderWindow window;
	Game();
	~Game();
	bool gameOver, gameQuit, ButtonEasyState, ButtonNormalState, ButtonHardState, ButtonBGState, ButtonSkinState, ButtonRestartState, ButtonQuitState;
	bool mouseDlbClkReady, RL_ClkJudge_flag;
	unsigned int Window_width, Window_height;
	int stageWidth, stageHeight, mMineNum, mFlagCalc, mouse_RL_ClkReady;
	int gamelvl, mTime;  //游戏难度，游戏计时
	MINE mGameData[LVL3_HEIGHT][LVL3_WIDTH];		//数组取最高难度的舞台尺寸
	bool isGameBegin, isMineSetBegin;		//游戏是否开始
	bool testMode;
	int isGameOverState;	//游戏结束的状态
	sf::Vector2i mCornPoint;	//舞台的左顶点坐标
	Vector2i P1, P2;//20200322 两处位置快速点击触发双击的BUG修订
	Vector2i RL_Point;//2020416

	int gridSize;	//块的大小
	int imgBGNo, imgSkinNo;

	Texture tBackground, tTiles, tButtons, tNum, tTimer, tCounter, tGameOver;		//	创建纹理对象
	Sprite  sBackground, sTiles, sButtons, sNum, sTimer, sCounter, sGameOver;       //	创建精灵对象
	sf::IntRect ButtonRectEasy, ButtonRectNormal, ButtonRectHard, ButtonRectBG, ButtonRectSkin, ButtonRectRestart, ButtonRectQuit;

	SoundBuffer sbWin, sbBoom;
	Sound soundWin, soundBoom;
	Music bkMusic;

	//SFML的Clock类在对象实例化的时候即开始计时
	sf::Clock gameClock, mouseClickTimer;

	void Run();

	void Initial();
	void IniData();
	void LoadMediaData();
	void MineSet(int Py, int Px);		//布雷

	void Input();
	void RButtonDown(Vector2i mPoint);		//鼠标右击
	void LButtonDown(Vector2i mPoint);		//鼠标左击
	void LButtonDblClk(Vector2i mPoint);		//鼠标左键双击
	void RL_ButtonDown(Vector2i mPoint);		//鼠标左右击

	void NullClick(int j, int i);	//查找空块

	void Logic();
	void RL_ClkJudge();
	void isWin();
	void unCover();
	void undownOpen();

	void Draw();
	void DrawGrid();
	void DrawButton();
	void DrawScore();
	void DrawTimer();
	void DrawGameEnd();

};

