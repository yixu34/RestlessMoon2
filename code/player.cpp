#include "player.hpp"

Player::Player()
{
}
Player::Player(const AnimatedEntityPtr& entity)
{
	_entity = entity;
}

Player::~Player(void)
{
}
