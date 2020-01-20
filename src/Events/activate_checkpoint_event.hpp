#pragma once

class ActivateCheckpointEvent
{
public:
    ActivateCheckpointEvent(Entity activated_checkpoint) : activated_checkpoint(activated_checkpoint) {}
    Entity activated_checkpoint;
};
