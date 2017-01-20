#include "CreatureSpawnList.h"

CNtlVector	CreatureSpawnList::GetPosition()
{
	return this->vCurLoc;
}
void		CreatureSpawnList::SetPosition(CNtlVector curPos)
{
	this->vCurLoc = curPos;
}
void		CreatureSpawnList::SetID(int _id)
{
	this->ID = _id;
}
int			CreatureSpawnList::GetID()
{
	return this->ID;
}
void		CreatureSpawnList::SetIsSpawn(bool wat)
{
	this->isSpawn = wat;
}
bool		CreatureSpawnList::isSpawned()
{
	return this->isSpawn;
}