#include "Ogmo.h"
#include "../../Input.h"
#include "../../Level.h"
#include "../../Monocle.h"

namespace Ogmo
{
	// T H E   P L A Y E R (entity)
	Player::Player(int x, int y) : Entity(),
		FRICTION_GROUND(0.90f), FRICTION_AIR(0.95f), GRAVITY(4.0f),
		JUMP(1.40f), ACCELERATION(10.0f), doubleJump(false), cling(0),
		tag("PLAYER"), onGround(false)
	{
		position = Vector2(x, y);

		AddTag(tag);
		SetCollider(new RectangleCollider(8, 8));

		sprite = new Sprite("player.png", FILTER_NONE, 8, 8);
		SetLayer(-1);
		SetGraphic(sprite);
	}

	void Player::Update()
	{
		// GRAB INPUT AND ACCELERATE
		if (Input::IsKeyMaskHeld("left"))
		{
			if(cling < 0) { velocity.x -= ACCELERATION * Monocle::deltaTime; }
		}
		else if (Input::IsKeyMaskHeld("right"))
		{
			if(cling < 0) { velocity.x += ACCELERATION * Monocle::deltaTime; }
		}

		// JUMP INPUT
		if (Input::IsKeyMaskPressed("jump") && (onGround || cling > 0 || !doubleJump))
		{
			// jump
			velocity.y = - JUMP;

			if(!onGround)
			{
				// wall jump
				if(cling > 0)
				{
					velocity.x = clingDir * 2;
					cling = -1;
				}

				// double jump
				if(cling < 0)
				{
					doubleJump = true;
				}
			}
		}
		
		// friction
		if(onGround) { velocity.x *= FRICTION_GROUND; }
		else { velocity.x *= FRICTION_AIR; }

		// gravity
		velocity.y += GRAVITY * Monocle::deltaTime;

		// Motion
		Motion(velocity.x, position.x);
		Motion(velocity.y, position.y);

		//check for ground
		if(velocity.y != 0)
		{
			if(CollideAt("WALL", position.x, position.y + 1))
			{
				onGround = true;
				doubleJump = false;
			}
			else
			{
				onGround = false;
			}
		}

		//check for wall jump
		if(!onGround)
		{
			if(CollideAt("WALL", position.x + 1, position.y) && Input::IsKeyMaskHeld("right")) 
			{ 
				cling = 10; 
				clingDir = -1; 
			}
			if(CollideAt("WALL", position.x - 1, position.y) && Input::IsKeyMaskHeld("left")) 
			{ 
				cling = 10; 
				clingDir = 1; 
			}
		}

		//decrease how long we can cling to a wall for
		cling --;
	}

	bool Player::Motion(float &speed, float &to)
	{
		// move
		to += speed;

		// collide
		bool col = false;
		while(Collide("WALL")) { to -= Sign(speed, 0.1); col = true; }
		
		// stop motion on collision
		if(col) { speed = 0; }

		// return
		return col;
	}


	bool Player::CollideAt(const std::string &tag, float x, float y)
	{
		Vector2 pre = Vector2(position.x, position.y);
		bool collide = false;

		position.x = x;
		position.y = y;

		if(Collide(tag)) { collide = true; }

		position = Vector2(pre.x, pre.y);
		return collide;
	}

	float Player::Sign(float i, float to)
	{
		return i < 0 ? - to : (i > 0 ? to : 0);
	}

	// T H E   W A L L (entity)
	Wall::Wall(int x, int y) : Entity()
	{
		position = Vector2(x, y);
		AddTag("WALL");
		SetCollider(new RectangleCollider(8, 8));
		SetLayer(5);
	}

	void Wall::Render()
	{
		Graphics::PushMatrix();
		Graphics::Translate(position);
		Graphics::BindTexture(NULL);
		Graphics::RenderQuad(8, 8);
		Graphics::PopMatrix();
	}

	// T H E   W O R L D (scene)
	void World::Begin()
	{
		//set screen size
		Graphics::Set2D(160, 120);
		Graphics::SetCameraPosition(Vector2(80, 60));

		//assets
		Assets::SetContentPath("../../Content/Ogmo/");

		//controls
		Input::DefineMaskKey("jump", KEY_UP);
		Input::DefineMaskKey("jump", KEY_Z);
		Input::DefineMaskKey("jump", KEY_A);

		Input::DefineMaskKey("left", KEY_LEFT);
		Input::DefineMaskKey("right", KEY_RIGHT);

		//eventually this will load the level
		Level::SetScene(this);
		Level::LoadProject("project.xml");
		Level::Load("level01.xml");

		//add player
		player = new Player(120, 8);
		Add(player);

		//add walls
		for(int i = 0; i < 160; i += 8)
		{
			wall = new Wall(i + 4, 116);
			Add(wall);
		}
		for(int i = 0; i < 120; i += 8)
		{
			wall = new Wall(4, i + 4);
			Add(wall);
		}

		for(int i = 0; i < 80; i += 8)
		{
			wall = new Wall(60, i + 4);
			Add(wall);
		}

		for(int i = 0; i < 18; i += 8)
		{
			wall = new Wall(12 + i, 28);
			Add(wall);
		}
	}

	void World::End()
	{
		delete player;
	}
}