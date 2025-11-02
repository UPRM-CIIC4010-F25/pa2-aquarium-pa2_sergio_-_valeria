#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::PowerUpFish:
            return "PowerUpFish";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 0.0f, 1, sprite) {
    if(sprite){
        float radius = std::max(sprite->getWidth(), sprite->getHeight()) / 2.0f;
        this->setCollisionRadius(radius);
    }else {
        this->setCollisionRadius(10.0f);
    }
}

void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    if (m_powerup_timer > 0) {
        m_powerup_timer--;
        if (m_powerup_timer == 0) {
            // Reset speed when power-up ends
            m_speed = m_original_speed;
            ofLogNotice() << "PowerUp expired!" << std::endl;
        }
    }
    this->reduceDamageDebounce();
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        float offset = m_collisionRadius;
        m_sprite->draw(m_x - offset, m_y - offset)
;    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    if (m_powerup_timer == 0) {
        m_original_speed = m_speed; // Store original speed
    }
    m_speed = speed;
    m_powerup_timer = 300; // Power-up lasts for 300 frames
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    do{
        m_dx = (rand() % 3 - 1); // -1, 0, or 1
        m_dy = (rand() % 3 - 1); // -1, 0, or 1
    } while (m_dx == 0 && m_dy == 0);

    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        float offset = m_collisionRadius;
        m_sprite->draw(m_x - offset, m_y - offset);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
   do{
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
   } while ( m_dx == 0 && m_dy == 0);

    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
        
        float offset = m_collisionRadius;
        m_sprite->draw(m_x - offset, m_y - offset);
}

PowerUpFish::PowerUpFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
   do{
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
   } while ( m_dx == 0 && m_dy == 0);

    normalize();

    setCollisionRadius(30); // PowerUp fish have a larger collision radius
    m_value = 0; // 
    m_creatureType = AquariumCreatureType::PowerUpFish;
}

void PowerUpFish::move() {
    // PowerUp fish might move fastest
    m_x += m_dx * (m_speed * 1.25); // Moves at 1.25x speed
    m_y += m_dy * (m_speed * 1.25);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void PowerUpFish::draw() const {
    ofLogVerbose() << "PowerUpFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
        
        float offset = m_collisionRadius;
        m_sprite->draw(m_x - offset, m_y - offset);
}

// ZigZagFish Implementation
ZigZagFish::ZigZagFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_creatureType = AquariumCreatureType::ZigZagFish;
    setCollisionRadius(30); // ZigZag fish have a standard collision radius
    m_value = 2;
}

void ZigZagFish::move() {
    zigzag_phase += 0.15f;
    m_x += m_dx * m_speed;
    m_y += sin(zigzag_phase) * 3.0f; // Zigzag effect
    bounce();
    if (m_dx < 0) m_sprite->setFlipped(true);
    else m_sprite->setFlipped(false);
}

void ZigZagFish::draw() const {
    ofLogVerbose() << "ZigZagFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;

    float offset = m_collisionRadius;
    m_sprite->draw(m_x - offset, m_y - offset);
}

// ChaserFish Implementation
ChaserFish::ChaserFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_creatureType = AquariumCreatureType::ChaserFish;
    setCollisionRadius(40); // Chaser fish have a larger collision radius
    m_value = 3;
}

void ChaserFish::move() {
    if (auto player = m_target){
        float dx = player->getX() - m_x;
        float dy = player->getY() - m_y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < 200.0f) {
            m_dx = dx / distance;
            m_dy = dy / distance;
            normalize();
        } else if (ofRandom(1.0f) < 0.02f) {
            m_dx = ofRandom(-1.0f, 1.0f);
            m_dy = ofRandom(-1.0f, 1.0f);
            normalize();
        }
    }
    m_x += m_dx * m_speed * 0.8f; // Slightly slower speed
    m_y += m_dy * m_speed * 0.8f;
    bounce();
    if (m_dx < 0) m_sprite->setFlipped(true);
    else m_sprite->setFlipped(false);
}

void ChaserFish::draw() const {
    ofLogVerbose() << "ChaserFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;

    float offset = m_collisionRadius;
    m_sprite->draw(m_x - offset, m_y - offset);
}

// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_powerup_fish = std::make_shared<GameSprite>("powerup-fish.png", 50, 50);
    this->m_player_fish = std::make_shared<GameSprite>("player-fish.png", 70,70);
    this->m_zigzag_fish = std::make_shared<GameSprite>("zigzag-fish.png", 60,60);
    this->m_chaser_fish = std::make_shared<GameSprite>("chaser-fish.png", 80,80);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);

        case AquariumCreatureType::PowerUpFish:
            return std::make_shared<GameSprite>(*this->m_powerup_fish);

        case AquariumCreatureType::ZigZagFish:
            return std::make_shared<GameSprite>(*this->m_zigzag_fish);

        case AquariumCreatureType::ChaserFish:
            return std::make_shared<GameSprite>(*this->m_chaser_fish);

        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }
    Repopulate();

    for(size_t i = 0; i < m_creatures.size(); ++i){
        for(size_t j = i + 1; j < m_creatures.size(); ++j){
            auto a = m_creatures[i];
            auto b = m_creatures[j];

            if (a && b && checkCollision(a, b)){
                a->bounce();
                b->bounce();

                GameEvent collisionEvent(GameEventType::COLLISION, a, b);
                collisionEvent.print();
            }
        }
    }

    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = 0;
    int y = 0;
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    float collisionRadius = 20;
    if (type == AquariumCreatureType::NPCreature) {
        collisionRadius = 30;
    } else if (type == AquariumCreatureType::PowerUpFish) {
        collisionRadius = 30;
    } else if (type == AquariumCreatureType::BiggerFish) {
        collisionRadius = 60;
    } else if (type == AquariumCreatureType::ZigZagFish) {
        collisionRadius = 30;
    } else if (type == AquariumCreatureType::ChaserFish) {
        collisionRadius = 40;
    }
    x = collisionRadius + rand() % static_cast<int>(m_width - 2 * collisionRadius);
    y = collisionRadius + rand() % static_cast<int>(m_height - 2 * collisionRadius);

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::PowerUpFish:
            this->addCreature(std::make_shared<PowerUpFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::PowerUpFish)));
            break;
        case AquariumCreatureType::ZigZagFish:
            this->addCreature(std::make_shared<ZigZagFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::ZigZagFish)));
            break;
        case AquariumCreatureType::ChaserFish: {
            auto chaser = std::make_shared<ChaserFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::ChaserFish));
            if (m_player) {
                chaser->setTarget(m_player);
            }
            this->addCreature(chaser);
            break;
        }
            
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();
    event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
    if (this->updateControl.tick()) {
        if (event != nullptr && event->isCollisionEvent()) {
            
            ofLogNotice() << "Collision detected between player and NPC!" << std::endl;
            if(event->creatureB != nullptr){
                event->print();

                float overlapX = m_player->getX() - event->creatureB->getX();
                float overlapY = m_player->getY() - event->creatureB->getY();

                event->creatureB->setDirection(-overlapX, -overlapY);
                event->creatureB->normalize();
                event->creatureB->move();

                auto npcCreature = std::static_pointer_cast<NPCreature>(event->creatureB);

                switch (npcCreature->GetType()){
                    case AquariumCreatureType::PowerUpFish:
                        ofLogNotice() << "Player collected a PowerUpFish!" << std::endl;
                        this->m_player->changeSpeed(this->m_player->getSpeed() * 1.1);
                        this->m_aquarium->removeCreature(event->creatureB);
                        break;
                    default:
                        break;
                }


                if(this->m_player->getPower() < event->creatureB->getValue()){
                    ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                    this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                    if(this->m_player->getLives() <= 0){
                        this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                        return;
                    }
                } else {
                    this->m_aquarium->removeCreature(event->creatureB);
                    this->m_player->addToScore(1, event->creatureB->getValue());
                    if (this->m_player->getScore() % 25 == 0){
                        this->m_player->increasePower(1);
                        ofLogNotice() << "Player power increased to " << this->m_player->getPower() << "!" << std::endl;
                    }
                }
                
                

            } else {
                ofLogError() << "Error: creatureB is null in collision event." << std::endl;
            }

        } else if (event->isCreatureAddedEvent()) {

        }
        this->m_aquarium->update();
    }

}

void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}

std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
