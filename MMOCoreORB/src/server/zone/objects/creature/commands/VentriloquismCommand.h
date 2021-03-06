/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef VENTRILOQUISMCOMMAND_H_
#define VENTRILOQUISMCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class VentriloquismCommand : public QueueCommand {
public:

	VentriloquismCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (!creature->isEntertaining()) {
			creature->sendSystemMessage("@performance:effect_not_performing");
			return GENERALERROR;
		}

		if (!creature->isPlayingMusic()) {
			creature->sendSystemMessage("@performance:effect_not_performing_correct");
			return GENERALERROR;
		}

		int actionModifier = Integer::valueOf(arguments.toString());

		if (actionModifier > 3 || actionModifier < 1)
			actionModifier = 3;

		StringBuffer effect;
		effect << "clienteffect/entertainer_ventriloquism_level_" << dec << actionModifier << ".cef";

		uint64 selectedTarget = creature->getTargetID();
 		ManagedReference<CreatureObject*> targetCreature = server->getZoneServer()->getObject(selectedTarget).castTo<CreatureObject*>();

 		if(targetCreature == NULL) {
			creature->sendSystemMessage("@performance:effect_need_target");
			return GENERALERROR;
		}
		if(targetCreature == creature || !targetCreature->isPlayerCreature())
			return GENERALERROR;


		int actionCost = 30 * actionModifier;
		actionCost = creature->calculateCostAdjustment(CreatureAttribute::QUICKNESS, actionCost);
		if (creature->getHAM(CreatureAttribute::ACTION) <= actionCost) {
			creature->sendSystemMessage("@performance:effect_too_tired");
			return GENERALERROR;
		}
		creature->inflictDamage(creature, CreatureAttribute::ACTION, actionCost, true);

		targetCreature->playEffect(effect.toString(), "");

		creature->sendSystemMessage("@performance:effect_perform_ventriloquism");

		return SUCCESS;
	}

};

#endif //VENTRILOQUISMCOMMAND_H_
