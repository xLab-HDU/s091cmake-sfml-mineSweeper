#include "Game.h"

Game::Game()
	:tBackground(), tTiles(), tButtons(), tNum(), tTimer(), tCounter(), tGameOver(),
	sBackground(tBackground), sTiles(tTiles), sButtons(tButtons), sNum(tNum), sTimer(tTimer), sCounter(tCounter), sGameOver(tGameOver),
	soundWin(sbWin), soundBoom(sbBoom)
{
	Window_width = 860;
	Window_height = 600;

	gamelvl = 2;
	imgBGNo = 1;
	imgSkinNo = 2;
	mouseDlbClkReady = false;
	mouse_RL_ClkReady = 0;
	RL_ClkJudge_flag = false;
	ButtonEasyState = false;
	ButtonNormalState = false;
	ButtonHardState = false;
	ButtonBGState = false;
	ButtonSkinState = false;
	ButtonRestartState = false;
	ButtonQuitState = false;
	//设定窗口属性，窗口禁止缩放
	uint8_t WindowStyle = sf::Style::Close | sf::Style::Titlebar;
	window.create(sf::VideoMode({ Window_width, Window_height }), L"MineSweeper by 李仕", WindowStyle);
}

Game::~Game()
{
}

void Game::Initial()
{
	window.setFramerateLimit(60); //每秒设置目标帧数
	gridSize = GRIDSIZE; //点击的位置的块的大小

	switch (gamelvl)
	{
	case 1:
		stageWidth = LVL1_WIDTH;
		stageHeight = LVL1_HEIGHT;
		mMineNum = LVL1_NUM;
		gridSize = GRIDSIZE * LVL2_WIDTH / LVL1_WIDTH; //简单难度的格子要放大，与难度中保持一致
		break;
	case 2:
		stageWidth = LVL2_WIDTH;
		stageHeight = LVL2_HEIGHT;
		mMineNum = LVL2_NUM;
		break;
	case 3:
		stageWidth = LVL3_WIDTH;
		stageHeight = LVL3_HEIGHT;
		mMineNum = LVL3_NUM;
		break;
	default:
		break;
	}
	gameOver = false;
	gameQuit = false;
	testMode = false;
	isGameOverState = ncNo;	//初始化游戏的结束状态
	mFlagCalc = 0;			//初始化旗子的数量
	isGameBegin = false;	//初始化游戏是否开始
	isMineSetBegin = false;	//初始化游戏是否开始布雷
	mTime = 0;				//初始化游戏进行的时间

	mCornPoint.x = (Window_width - stageWidth * gridSize) / 2;		//设置舞台的左上角坐标
	mCornPoint.y = (Window_height - stageHeight * gridSize) / 2;
	IniData();		//初始化数据
	LoadMediaData();	//加载素材
}
void Game::IniData()
{
	int i, j;

	for (j = 0; j < stageHeight; j++)		//所有块置为空且未点击
		for (i = 0; i < stageWidth; i++)
		{
			mGameData[j][i].mState = ncUNDOWN;
			mGameData[j][i].mStateBackUp = ncUNDOWN;
			mGameData[j][i].isPress = false;
		}
}
void Game::LoadMediaData()
{
	std::stringstream ss;
	ss << "../data/images/BK0" << imgBGNo << ".jpg";
	if (!tBackground.loadFromFile(ss.str()))
	{
		std::cout << "BK image 没有找到" << std::endl;
	}
	ss.str(""); //清空字符串
	ss << "../data/images/Game" << imgSkinNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
	{
		std::cout << "Game Skin image 没有找到" << std::endl;
	}

	if (!tNum.loadFromFile("../data/images/num.jpg"))
	{
		std::cout << "num.jpg 没有找到" << std::endl;
	}
	if (!tTimer.loadFromFile("../data/images/jishiqi.jpg"))
	{
		std::cout << "jishiqi.jpg 没有找到" << std::endl;
	}
	if (!tCounter.loadFromFile("../data/images/jishuqi.jpg"))
	{
		std::cout << "jishuqi.jpg 没有找到" << std::endl;
	}
	if (!tButtons.loadFromFile("../data/images/button.jpg"))
	{
		std::cout << "button.jpg 没有找到" << std::endl;
	}
	if (!tGameOver.loadFromFile("../data/images/gameover.jpg"))
	{
		std::cout << "gameover.jpg 没有找到" << std::endl;
	}
	sBackground.setTexture(tBackground, true);
	sTiles.setTexture(tTiles, true);
	if (gamelvl == 1)
	{
		float scale = 1.0 * LVL2_WIDTH / LVL1_WIDTH;
		sTiles.setScale({ scale, scale });
	}
	else
	{
		sTiles.setScale({ 1.0, 1.0 });//栅格尺寸取消缩放
	}

	sButtons.setTexture(tButtons, true);
	sNum.setTexture(tNum, true);
	sTimer.setTexture(tTimer, true);
	sCounter.setTexture(tCounter, true);
	sGameOver.setTexture(tGameOver, true);

}
void Game::MineSet(int Py, int Px)		//布雷
{
	int mCount, i, j, k, l;
	mCount = 0;

	srand(time(NULL));		//用当前系统时间作为随机数生成器的种子
	//布雷
	//第一种布雷方案20191218
	/*do {
		k = rand() % stageHeight;  //生成随机数
		l = rand() % stageWidth;
		if(k == Py && l== Px)
			continue;
		if (mGameData[k][l].mState == ncUNDOWN)
		{
			mGameData[k][l].mState = ncMINE;
			mGameData[k][l].mStateBackUp = ncMINE;  //备份状态
			mCount++;
		}
	} while (mCount != mMineNum);*/
	//因为有同学反映，前面的布雷代码，有时候第一下只能点开一个方格，没被爽到！所以给出第二种布雷方案20191218
	/*do {
		bool flag = true;//当次循环是否布雷的判定变量
		k = rand() % stageHeight;  //生成随机数
		l = rand() % stageWidth;
		for (i = Py - 1; i < Py + 2; i++)	//鼠标第一下点击处及周围8领域
			for (j = Px - 1; j < Px + 2; j++)
				if (i >= 0 && i < stageHeight && j >= 0 && j < stageWidth)//9宫格取处于舞台中的部分进行后续判定
					if (k == i && l == j)
						flag = false;//随机坐标若处于9宫格覆盖范围，则不要布雷
		if (flag && mGameData[k][l].mState == ncUNDOWN)
		{
			mGameData[k][l].mState = ncMINE;
			mGameData[k][l].mStateBackUp = ncMINE;  //备份状态
			mCount++;
		}
	} while (mCount != mMineNum);*/
	//第三种布雷方案20200417
	do {
		k = rand() % stageHeight;  //生成随机数
		l = rand() % stageWidth;
		if (k >= Py - 1 && k <= Py + 1 && l >= Px - 1 && l <= Px + 1)
			continue;
		if (mGameData[k][l].mState == ncUNDOWN)
		{
			mGameData[k][l].mState = ncMINE;
			mGameData[k][l].mStateBackUp = ncMINE;  //备份状态
			mCount++;
		}
		if (mGameData[k][l].mState == ncFLAG || mGameData[k][l].mState == ncQ)
		{
			mGameData[k][l].mStateBackUp = ncMINE;  //备份状态
			mCount++;
		}
	} while (mCount != mMineNum);
	//方格赋值
	for (i = 0; i < stageHeight; i++)
		for (j = 0; j < stageWidth; j++)
		{
			if (mGameData[i][j].mStateBackUp != ncMINE)
			{
				mCount = 0;
				for (k = i - 1; k < i + 2; k++)
					for (l = j - 1; l < j + 2; l++)
						if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
						{
							if (mGameData[k][l].mStateBackUp == ncMINE)
								mCount++;
						}//计算(i,j)周围雷的数目
				switch (mCount)//保存状态
				{
				case 0:
					mGameData[i][j].mStateBackUp = ncNULL;
					break;
				case 1:
					mGameData[i][j].mStateBackUp = ncONE;
					break;
				case 2:
					mGameData[i][j].mStateBackUp = ncTWO;
					break;
				case 3:
					mGameData[i][j].mStateBackUp = ncTHREE;
					break;
				case 4:
					mGameData[i][j].mStateBackUp = ncFOUR;
					break;
				case 5:
					mGameData[i][j].mStateBackUp = ncFIVE;
					break;
				case 6:
					mGameData[i][j].mStateBackUp = ncSIX;
					break;
				case 7:
					mGameData[i][j].mStateBackUp = ncSEVEN;
					break;
				case 8:
					mGameData[i][j].mStateBackUp = ncEIGHT;
					break;
				}
				if (mGameData[i][j].mState != ncFLAG && mGameData[i][j].mState != ncQ)
					mGameData[i][j].mState = mGameData[i][j].mStateBackUp;

			}

		}
}

void Game::Input()
{
	while (const std::optional event = window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			window.close();		//窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
			gameQuit = true;
		}
		// 按键事件
		if (const auto* KeyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			if (KeyReleased->scancode == sf::Keyboard::Scancode::Escape)
			{
				window.close();  //窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
				gameQuit = true;
			}
		}
		// 鼠标事件
		if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
		{
			if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
				if (isGameOverState == ncNo)
				{//20200423
					if (mouseClickTimer.getElapsedTime().asMilliseconds() > 700)//如果间隔大于500ms则是单击
					{
						mouseClickTimer.restart();//重新计时
						P2 = Mouse::getPosition(window);//获取鼠标位置

						if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
							RL_ButtonDown(P2);//-----》鼠标左右击
						else
						{
							LButtonDown(P2);//或鼠标左键单击
							mouseDlbClkReady = true;//20200416						
						}
					}
					mouse_RL_ClkReady++;
				}
			}

			if (mouseButtonPressed->button == sf::Mouse::Button::Left)
			{
				if (isGameOverState == ncNo)
				{//20200423
					if (mouseDlbClkReady)
						mouseDlbClkReady = false;
					else
					{
						P1 = Mouse::getPosition(window);
						if (mouseClickTimer.getElapsedTime().asMilliseconds() < 700 && P2.x - P1.x < gridSize / 4 && P2.y - P1.y < gridSize / 4
							&& P2.x - P1.x > -gridSize / 4 && P2.y - P1.y > -gridSize / 4)
							LButtonDblClk(P2);//当两次点击的间隔小于500毫秒，则判定为鼠标双击
					}

					mouse_RL_ClkReady = 0;//20200423

					//按钮判断
					if (isGameBegin == false)
					{

						if (ButtonRectEasy.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
							gamelvl = 1;
						if (ButtonRectNormal.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
							gamelvl = 2;
						if (ButtonRectHard.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
							gamelvl = 3;
						Initial();//及时刷新舞台

					}
				}
				if (ButtonRectBG.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
				{
					imgBGNo++;
					if (imgBGNo > 7)//大于背景图的总数时候
						imgBGNo = 1;//重新轮换背景图
					LoadMediaData();
				}
				if (ButtonRectSkin.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
				{
					imgSkinNo++;
					if (imgSkinNo > 6)//大于皮肤图的总数时候
						imgSkinNo = 1;//重新轮换皮肤图
					LoadMediaData();
				}
				if (ButtonRectRestart.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
				{
					Initial();
				}
				if (ButtonRectQuit.contains({ mouseButtonPressed->position.x, mouseButtonPressed->position.y }))
				{
					window.close();  //窗口可以移动、调整大小和最小化。但是如果要关闭，需要自己去调用close()函数
					gameQuit = true;
				}
			}

			if (mouseButtonPressed->button == sf::Mouse::Button::Right)
			{
				if (isGameOverState == ncNo)
				{
					mouse_RL_ClkReady++;

					if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
						RL_ButtonDown(Mouse::getPosition(window));//-----》鼠标左右击20200416
					else
						RButtonDown(Mouse::getPosition(window));//-----》鼠标右击
				}
			}
		}
		if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
		{
			mouse_RL_ClkReady = 0;//20200416
		}
		if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
		{
			if (isGameBegin == false)
			{
				if (ButtonRectEasy.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonEasyState = true;
				else
					ButtonEasyState = false;
				if (ButtonRectNormal.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonNormalState = true;
				else
					ButtonNormalState = false;
				if (ButtonRectHard.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonHardState = true;
				else
					ButtonHardState = false;
				if (ButtonRectBG.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonBGState = true;
				else
					ButtonBGState = false;
				if (ButtonRectSkin.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonSkinState = true;
				else
					ButtonSkinState = false;
				if (ButtonRectRestart.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonRestartState = true;
				else
					ButtonRestartState = false;
				if (ButtonRectQuit.contains({ mouseMoved->position.x, mouseMoved->position.y }))
					ButtonQuitState = true;
				else
					ButtonQuitState = false;
			}
		}
		// 按键事件
		if (const auto* KeyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			if (KeyReleased->scancode == sf::Keyboard::Scancode::T)
			{
				if (testMode == false)
				{
					testMode = true;
					for (int i = 0; i < stageHeight; i++)
						for (int j = 0; j < stageWidth; j++)
						{
							mGameData[i][j].isPressBackUp = mGameData[i][j].isPress;
							mGameData[i][j].isPress = true;
						}
				}
				else
				{
					testMode = false;
					for (int i = 0; i < stageHeight; i++)
						for (int j = 0; j < stageWidth; j++)
						{
							mGameData[i][j].isPress = mGameData[i][j].isPressBackUp;
						}
				}
			}
		}
	}
}
void Game::RButtonDown(Vector2i mPoint)		//鼠标右击
{
	int i, j;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight)//如果点击是在舞台范围内
	{
		if (isGameBegin == false)//如果游戏未开始
		{
			isGameBegin = true;//游戏开始
			gameClock.restart();
		}
		if (mGameData[j][i].isPress == false)
		{
			mGameData[j][i].isPress = true;
			//mGameData[j][i].mStateBackUp = mGameData[j][i].mState;
			mGameData[j][i].mState = ncFLAG;
			mFlagCalc++;
		}
		else
		{
			if (mGameData[j][i].mState == ncFLAG)
			{
				mGameData[j][i].isPress = true;
				mGameData[j][i].mState = ncQ;
				mFlagCalc--;
			}
			else if (mGameData[j][i].mState == ncQ)
			{
				mGameData[j][i].isPress = false;
				mGameData[j][i].mState = mGameData[j][i].mStateBackUp;
			}
		}
	}
}
void Game::RL_ButtonDown(Vector2i mPoint)		//鼠标左右击
{
	int i, j, k, l;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //如果点击是在范围内
	{
		if (mGameData[j][i].isPress == true)//如果已被点击
		{
			if (mGameData[j][i].mState != ncFLAG && mGameData[j][i].mState != ncQ)//如果当前块不是旗子
				for (k = j - 1; k < j + 2; k++)
					for (l = i - 1; l < i + 2; l++)//遍历周围8个格子
						if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
						{
							if (mGameData[k][l].isPress == false)//如果8领域格子未被点击
							{
								mGameData[k][l].isPress = true;
								//mGameData[k][l].mStateBackUp = mGameData[k][l].mState;
								mGameData[k][l].mState = ncX;
							}
						}
		}
		else//微软规则，可以左右键双击未点击的格子，使得RL双击的优先级“好像”高于单击
		{
			for (k = j - 1; k < j + 2; k++)
				for (l = i - 1; l < i + 2; l++)//遍历周围8个格子
					if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
					{
						if (mGameData[k][l].isPress == false)//如果8领域格子未被点击
						{
							mGameData[k][l].isPress = true;
							//mGameData[k][l].mStateBackUp = mGameData[k][l].mState;
							mGameData[k][l].mState = ncX;
						}
					}
			mGameData[j][i].isPress = false;//微软规则，点击的格子，状态不变
		}
	}
	RL_Point = mPoint;
	RL_ClkJudge_flag = true;
}
void Game::RL_ClkJudge()//左右键双击的判定20200416
{
	int i, j, k, l, mineNum = 0, flagNum = 0;
	i = (RL_Point.x - mCornPoint.x) / gridSize;
	j = (RL_Point.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //如果点击是在范围内
	{
		if (mGameData[j][i].isPress == true)//如果已被点击
		{
			if (mGameData[j][i].mState != ncFLAG)//如果当前块不是旗子
				for (k = j - 1; k < j + 2; k++)
					for (l = i - 1; l < i + 2; l++)//遍历周围8个格子
						if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
						{
							if (mGameData[k][l].mState == ncFLAG)
								flagNum++;
							if (mGameData[k][l].mState == ncX)//如果8领域格子状态是ncX，则取消
							{
								mGameData[k][l].isPress = false;
								mGameData[k][l].mState = mGameData[k][l].mStateBackUp;
							}
						}
		}
		else
		{
			for (k = j - 1; k < j + 2; k++)
				for (l = i - 1; l < i + 2; l++)//遍历周围8个格子
					if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
					{
						if (mGameData[k][l].mState == ncX)//如果8领域格子状态是ncX，则取消
						{
							mGameData[k][l].isPress = false;
							mGameData[k][l].mState = mGameData[k][l].mStateBackUp;
						}
					}
		}
		if (mGameData[j][i].mState == flagNum + 2)//数字1-8对应编号是3-10
			LButtonDblClk(RL_Point);
	}

	RL_ClkJudge_flag = false;
}
void Game::LButtonDown(Vector2i mPoint)		//鼠标左击
{
	int i, j;
	i = (mPoint.x - mCornPoint.x) / gridSize;	//	获取鼠标当前点击的块的位置
	j = (mPoint.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //如果点击是在舞台范围
	{
		if (isGameBegin == false)//如果游戏未开始
		{
			isGameBegin = true;//游戏开始
			gameClock.restart();
		}
		if (isMineSetBegin == false)//如果游戏未开始
		{
			isMineSetBegin = true;//游戏开始
			MineSet(j, i);		//点击之后再随机布雷
		}
		if (mGameData[j][i].mState != ncFLAG)//如果状态不是旗子
			if (mGameData[j][i].isPress == false)
			{
				mGameData[j][i].isPress = true;	//当前块被点击
				if (mGameData[j][i].mState == ncMINE)	//如果当前为雷
				{
					isGameBegin = false;
					isGameOverState = ncLOSE;
					mGameData[j][i].mState = ncBOMBING;
					unCover();	//揭开剩下未被找到的雷
				}
			}
		if (mGameData[j][i].mState == ncNULL) //如果当前块为空
			NullClick(j, i);		//查找未被点击的空块
	}
}
void Game::NullClick(int j, int i)	//查找空块
{
	int k, l;
	for (k = j - 1; k < j + 2; k++)
		for (l = i - 1; l < i + 2; l++)
			if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
			{
				if (mGameData[k][l].isPress == false)
				{
					mGameData[k][l].isPress = true;
					if (mGameData[k][l].mState == ncNULL)	//如果状态为空
						NullClick(k, l);					//递归调用,继续查找
				}
			}
}
void Game::LButtonDblClk(Vector2i mPoint)		//鼠标左键双击
{
	int i, j, k, l, lvl;
	i = (mPoint.x - mCornPoint.x) / gridSize;
	j = (mPoint.y - mCornPoint.y) / gridSize;

	if (i >= 0 && i < stageWidth && j >= 0 && j < stageHeight) //如果点击是在范围内
	{
		if (mGameData[j][i].isPress == true)//如果已被点击
		{
			if (mGameData[j][i].mState != ncFLAG)//如果当前块不是旗子
				for (k = j - 1; k < j + 2; k++)
					for (l = i - 1; l < i + 2; l++)//遍历周围8个格子
						if (k >= 0 && k < stageHeight && l >= 0 && l < stageWidth)
						{
							if (mGameData[k][l].mState == ncFLAG)//如果状态是旗子
							{
								if (mGameData[k][l].mStateBackUp != ncMINE)//如果原先状态不是雷
								{
									isGameOverState = ncLOSE;
									isGameBegin = false;
									unCover();
								}
							}
							else //如果状态不是旗子
							{
								if (mGameData[k][l].isPress == false)
								{
									mGameData[k][l].isPress = true;
									if (mGameData[k][l].mState == ncMINE)//如果为雷
									{
										isGameOverState = ncLOSE;
										isGameBegin = false;
										mGameData[k][l].mState = ncBOMBING;
										unCover();
									}
									if (mGameData[k][l].mState == ncNULL)//如果为空块
										NullClick(k, l);
								}
							}
						}
		}
	}
}

void Game::Logic()
{
	if (mouse_RL_ClkReady == 0 && RL_ClkJudge_flag == true)//20200416
		RL_ClkJudge();

	isWin();
}
void Game::isWin()
{
	int i, j, c = 0;

	if (isGameOverState != ncLOSE)
	{
		for (i = 0; i < stageWidth; i++)
			for (j = 0; j < stageHeight; j++)
			{	//这个条件很有意思，大家琢磨一下。如果有旗子误插了，那该条件有没可能成立？
				if (mGameData[j][i].isPress == false || mGameData[j][i].mState == ncFLAG)
					c++;
			}
	}

	if (c == mMineNum)//如果所有旗子对应都是雷，游戏胜利并结束
	{
		isGameBegin = false;
		mFlagCalc = mMineNum;
		undownOpen();
		isGameOverState = ncWIN;
	}
}

void Game::undownOpen()
{
	int i, j;
	for (j = 0; j < stageHeight; j++)
		for (i = 0; i < stageWidth; i++)
		{
			if (mGameData[j][i].isPress == false)
			{
				mGameData[j][i].isPress = true;
				if (mGameData[j][i].mState == ncMINE)
					mGameData[j][i].mState = ncFLAG;
			}

		}
}

void Game::unCover()
{
	int i, j;
	for (j = 0; j < stageHeight; j++)
		for (i = 0; i < stageWidth; i++)
		{
			if (mGameData[j][i].isPress == false)
				if (mGameData[j][i].mState == ncMINE)
				{
					mGameData[j][i].isPress = true;
					mGameData[j][i].mState = ncUNFOUND;
				}
		}
}
void Game::Draw()
{
	window.clear();	//清屏

	//绘制背景
	sBackground.setPosition({ 0.0, 0.0 });
	window.draw(sBackground);

	//绘制舞台
	DrawGrid();
	DrawButton();
	DrawScore();
	DrawTimer();

	if (isGameOverState)
		DrawGameEnd();

	window.display();//把显示缓冲区的内容，显示在屏幕上。SFML采用的是双缓冲机制
}
void Game::DrawGrid()
{
	int i, j;
	for (j = 0; j < stageHeight; j++)
		for (i = 0; i < stageWidth; i++)
		{
			if (mGameData[j][i].isPress == true)
			{
				sTiles.setTextureRect(IntRect({ mGameData[j][i].mState * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
				sTiles.setPosition({ static_cast<float>(mCornPoint.x + i * gridSize), static_cast<float>(mCornPoint.y + j * gridSize) });//简单难度的格子尺寸要放大
			}
			else
			{
				sTiles.setTextureRect(IntRect({ ncUNDOWN * GRIDSIZE, 0 }, { GRIDSIZE, GRIDSIZE }));
				sTiles.setPosition({ static_cast<float>(mCornPoint.x + i * gridSize), static_cast<float>(mCornPoint.y + j * gridSize) });
			}
			window.draw(sTiles);
		}
}
void Game::DrawButton()
{
	Vector2i LeftCorner;
	int ButtonWidth = 60;
	int ButtonHeight = 36;
	int detaX;
	detaX = (Window_width - ButtonWidth * 7) / 8;  //7个按钮在界面上等分宽度
	LeftCorner.y = Window_height - GRIDSIZE * 3;		//指定高度
	sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色

	//ButtonRectEasy
	LeftCorner.x = detaX;
	sButtons.setTextureRect(IntRect({ 0 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x),  static_cast<float>(LeftCorner.y) });								//设置按钮的位置坐标
	ButtonRectEasy.position.x = LeftCorner.x;
	ButtonRectEasy.position.y = LeftCorner.y;
	ButtonRectEasy.size.x = ButtonWidth;
	ButtonRectEasy.size.y = ButtonHeight;
	if (ButtonEasyState)
		sButtons.setColor(Color(255, 255, 255, 128));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
	//ButtonRectNormal
	LeftCorner.x = detaX * 2 + ButtonWidth;
	sButtons.setTextureRect(IntRect({ 1 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });//设置按钮的位置坐标
	ButtonRectNormal.position.x = LeftCorner.x;
	ButtonRectNormal.position.y = LeftCorner.y;
	ButtonRectNormal.size.x = ButtonWidth;
	ButtonRectNormal.size.y = ButtonHeight;
	if (ButtonNormalState)
		sButtons.setColor(Color(128, 255, 255, 255));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
	//ButtonRectHard
	LeftCorner.x = detaX * 3 + ButtonWidth * 2;
	sButtons.setTextureRect(IntRect({ 2 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });//设置按钮的位置坐标
	ButtonRectHard.position.x = LeftCorner.x;
	ButtonRectHard.position.y = LeftCorner.y;
	ButtonRectHard.size.x = ButtonWidth;
	ButtonRectHard.size.y = ButtonHeight;
	if (ButtonHardState)
		sButtons.setColor(Color(128, 128, 255, 255));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
	//ButtonRectBG
	LeftCorner.x = detaX * 4 + ButtonWidth * 3;
	sButtons.setTextureRect(IntRect({ 4 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });//设置按钮的位置坐标
	ButtonRectBG.position.x = LeftCorner.x;
	ButtonRectBG.position.y = LeftCorner.y;
	ButtonRectBG.size.x = ButtonWidth;
	ButtonRectBG.size.y = ButtonHeight;
	if (ButtonBGState)
		sButtons.setColor(Color(128, 255, 128, 255));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
	//ButtonRectSkin
	LeftCorner.x = detaX * 5 + ButtonWidth * 4;
	sButtons.setTextureRect(IntRect({ 3 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });//设置按钮的位置坐标
	ButtonRectSkin.position.x = LeftCorner.x;
	ButtonRectSkin.position.y = LeftCorner.y;
	ButtonRectSkin.size.x = ButtonWidth;
	ButtonRectSkin.size.y = ButtonHeight;
	if (ButtonSkinState)
		sButtons.setColor(Color(255, 255, 128, 255));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
	//ButtonRectRestart
	LeftCorner.x = detaX * 6 + ButtonWidth * 5;
	sButtons.setTextureRect(IntRect({ 5 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });//设置按钮的位置坐标
	ButtonRectRestart.position.x = LeftCorner.x;
	ButtonRectRestart.position.y = LeftCorner.y;
	ButtonRectRestart.size.x = ButtonWidth;
	ButtonRectRestart.size.y = ButtonHeight;
	if (ButtonRestartState)
		sButtons.setColor(Color(255, 128, 128, 255));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
	//ButtonRectQuit
	LeftCorner.x = detaX * 7 + ButtonWidth * 6;
	sButtons.setTextureRect(IntRect({ 6 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));//读取按钮的纹理区域
	sButtons.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });//设置按钮的位置坐标
	ButtonRectQuit.position.x = LeftCorner.x;
	ButtonRectQuit.position.y = LeftCorner.y;
	ButtonRectQuit.size.x = ButtonWidth;
	ButtonRectQuit.size.y = ButtonHeight;
	if (ButtonQuitState)
		sButtons.setColor(Color(128, 128, 128, 255));	//设置阴影的颜色
	else
		sButtons.setColor(Color(255, 255, 255, 255));//颜色改回原色
	window.draw(sButtons);
}
void Game::DrawScore()
{
	Vector2i LeftCorner;
	LeftCorner.x = Window_width - sCounter.getLocalBounds().size.x * 1.25;
	LeftCorner.y = sCounter.getLocalBounds().size.y * 0.5;
	sCounter.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });	//计数器纹理的贴图位置
	window.draw(sCounter);

	int NumSize = sNum.getLocalBounds().size.y;
	LeftCorner.x = LeftCorner.x + sCounter.getLocalBounds().size.x - NumSize;
	LeftCorner.y = LeftCorner.y + sCounter.getLocalBounds().size.y * 0.5 - NumSize * 0.5;

	int mScore = mMineNum - mFlagCalc;
	//绘制个位数的数字
	int a = mScore % 10;
	sNum.setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(sNum);
	//绘制十位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(sNum);
	//绘制百位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(sNum);
}
void Game::DrawTimer()
{
	Vector2i LeftCorner;
	LeftCorner.x = sTimer.getLocalBounds().size.x * 0.25;
	LeftCorner.y = sTimer.getLocalBounds().size.y * 0.5;
	sTimer.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });	//计时器纹理的贴图位置
	window.draw(sTimer);

	if (isGameBegin)
		mTime = gameClock.getElapsedTime().asSeconds();

	int NumSize = sNum.getLocalBounds().size.y;
	LeftCorner.x = LeftCorner.x + sTimer.getLocalBounds().size.x - NumSize * 1.5;
	LeftCorner.y = LeftCorner.y + sTimer.getLocalBounds().size.y * 0.5 - NumSize * 0.5;

	int mScore = mTime;
	if (mScore > 999)
		mScore = 999;
	//绘制个位数的数字
	int a = mScore % 10;
	sNum.setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(sNum);
	//绘制十位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(sNum);
	//绘制百位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum.setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(sNum);
}
void Game::DrawGameEnd()
{
	Vector2i LeftCorner;
	int ButtonWidth = 200;
	int ButtonHeight = sGameOver.getLocalBounds().size.y;
	LeftCorner.x = (Window_width - ButtonWidth) / 2;
	LeftCorner.y = (Window_height - ButtonHeight) / 2;

	sGameOver.setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });

	if (isGameOverState == ncWIN)
		sGameOver.setTextureRect(IntRect({ 0 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));
	if (isGameOverState == ncLOSE)
		sGameOver.setTextureRect(IntRect({ 1 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));

	window.draw(sGameOver);
}
void Game::Run()
{
	do
	{
		Initial();
		while (window.isOpen() && gameOver == false)
		{
			Input();

			Logic();

			Draw();
		}
	} while (!gameQuit);

}