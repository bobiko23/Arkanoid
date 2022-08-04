#include "Board.h"
#include "World.h"

extern World world;

Board::Board()
{
}

Board::~Board()
{
}

void Board::load()
{
	srand(time(0));
	
	fstream stream;

	string tmp, background, img;

	int2 dir;

	stream.open(CONFIG_FOLDER + GAME_FOLDER + "board.txt");

	stream >> tmp >> background;
	stream >> tmp >> dir.x >> dir.y;
	stream >> tmp >> m_space.rect.x >> m_space.rect.y >> m_space.rect.w >> m_space.rect.h;
	stream >> tmp >> img;
	stream >> tmp >> m_speed;

	stream.close();

	m_background = loadTexture(GAME_FOLDER + background);

	m_direction.first = (SDL_Scancode)dir.x; // Right
	m_direction.second = (SDL_Scancode)dir.y; // Left

	m_space.texture = loadTexture(GAME_FOLDER + img);

	m_brick.init();

	m_ball.init();

	m_drops.initAll();

	m_allDrops.clear();
}

void Board::update()
{
	m_ball.update();
	
	collLeftRight(m_ball.m_ball.rect, m_space.rect);
	collUpDown(m_ball.m_ball.rect, m_space.rect);
	
	for (int i = 0; i < m_ROWS; i++)
	{
		for (int j = 0; j < m_COLS; j++)
		{
			collLeftRight(m_ball.m_ball.rect, m_brick.m_allBricks[i][j].rect);
			collUpDown(m_ball.m_ball.rect, m_brick.m_allBricks[i][j].rect);

			if (collRectRect(m_brick.m_allBricks[i][j].rect, m_ball.m_ball.rect))
			{
				m_brick.m_allBricks[i][j].m_hp -= m_ball.m_dmg;

				if (m_brick.m_allBricks[i][j].m_hp <= 0)
				{	
					int r = rand() % 3;

					if (r == 1)
					{
						Dropable _drop = m_drops.createNew();
						
						_drop.m_drop.rect.x = m_brick.m_allBricks[i][j].rect.x;
						_drop.m_drop.rect.y = m_brick.m_allBricks[i][j].rect.y;

						m_allDrops.push_back(_drop);
					}
					
					SDL_DestroyTexture(m_brick.m_allBricks[i][j].texture);

					m_brick.m_allBricks[i][j].rect = {0, 0, 0, 0};

					m_brick.m_counter++;
				}

				if (m_brick.m_counter >= m_ROWS * m_COLS)
				{
					world.m_stateManager.changeGameState(GAME_STATE::WIN_SCREEN);
					
					return;
				}
			}
		}
	}

	for (int i = 0; i < m_allDrops.size(); i++)
	{
		m_allDrops[i].update();
		
		if (collRectRect(m_space.rect, m_allDrops[i].m_drop.rect))
		{
			if (m_allDrops[i].m_type == "BigBall")
			{				
				m_allDrops.erase(m_allDrops.begin() + i);
				
				m_ball.m_ball.rect.w += 20;
				m_ball.m_ball.rect.h += 20;

				return;
			}
			else if (m_allDrops[i].m_type == "Reverse")
			{
				m_allDrops.erase(m_allDrops.begin() + i);
				
				SDL_Scancode _tmp = m_direction.first;

				m_direction.first = m_direction.second;
				m_direction.second = _tmp;
				
				return;
			}
		}

		if (m_allDrops[i].m_drop.rect.y >= Presenter::m_SCREEN_HEIGHT)
		{
			m_allDrops.erase(m_allDrops.begin() + i);

			return;
		}
	}

	if (isKeyPressed(m_direction.first))
	{
		m_space.rect.x += m_speed;
		
		if (m_space.rect.x + m_space.rect.w >= Presenter::m_SCREEN_WIDTH)
		{
			m_space.rect.x = Presenter::m_SCREEN_WIDTH - m_space.rect.w;
		}	
	}
	else if(isKeyPressed(m_direction.second))
	{
		m_space.rect.x -= m_speed;

		if (m_space.rect.x <= 0)
		{
			m_space.rect.x = 0;
		}
	}
}

void Board::draw()
{
	drawObject(m_background);

	drawObject(m_space);

	m_ball.draw();

	m_brick.draw();

	for (int i = 0; i < m_allDrops.size(); i++)
	{
		m_allDrops[i].draw();
	}
}

void Board::collUpDown(SDL_Rect rect1, SDL_Rect rect2)
{
	if (rect1.y > rect2.y && rect1.y < rect2.y + rect2.h &&
		!(rect1.y + rect1.h > rect2.y && rect1.y + rect1.h < rect2.y + rect2.h) &&
		((rect2.x > rect1.x && rect2.x < rect1.x + rect1.w) ||
		(rect2.x + rect2.w > rect1.x && rect2.x + rect2.w < rect1.x + rect1.w)))
	{
		int2 center = { rect1.y + rect1.h / 2, rect2.y + rect2.h / 2 };
		
		m_ball.collisionY({abs(center.y - center.x), (rect2.h / 2)});
		
		rect1.y = rect2.y + rect2.h;
		
		return;
	}
	
	if (rect1.y + rect1.h > rect2.y && rect1.y + rect1.h < rect2.y + rect2.h &&
		!(rect1.y > rect2.y && rect1.y < rect2.y + rect2.h) &&
		((rect2.x > rect1.x && rect2.x < rect1.x + rect1.w)||
		(rect2.x + rect2.w > rect1.x && rect2.x + rect2.w < rect1.x + rect1.w)))
	{
		int2 center = { rect1.y + rect1.h / 2 , rect2.y + rect2.h / 2 };
		
		m_ball.collisionY({ abs(center.y - center.x), (rect2.h / 2) });
		
		rect1.y = rect2.y - rect1.h;
	}
}

void Board::collLeftRight(SDL_Rect rect1, SDL_Rect rect2)
{
	if (rect1.x > rect2.x && rect1.x < rect2.x + rect2.w &&
		((rect1.y > rect2.y && rect1.y < rect2.y + rect2.h)||
		(rect1.y + rect1.h > rect2.y && rect1.y + rect1.h < rect2.y + rect2.h)))
	{
		int2 center = { rect1.x + rect1.w / 2 , rect2.x + rect2.w / 2 };
		
		m_ball.collisionX({ abs(center.y - center.x), (rect2.w / 2) });
		
		rect1.x = rect2.x + rect2.w;
		
		return;
	}
	
	if (rect1.x + rect1.w > rect2.x && rect1.x + rect1.w < rect2.x + rect2.w &&
		((rect1.y > rect2.y && rect1.y < rect2.y + rect2.h)||
		(rect1.y + rect1.h > rect2.y && rect1.y + rect1.h < rect2.y + rect2.h)))
	{
		int2 center = { rect1.x + rect1.w / 2 , rect2.x + rect2.w / 2 };
		
		m_ball.collisionX({ abs(center.y - center.x), (rect2.w / 2) });
		
		rect1.x = rect2.x - rect1.w;
	}
}