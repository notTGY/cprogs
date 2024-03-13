class Game_Hero {
private:
    int health_; // здоровье
    bool isZombie_; // флаг, герой ли он или зомби
    bool isDead_; // флаг, мертвый ли герой

public:
    // конструктор
    Game_Hero(int health = 10, bool zombie = false) : health_(health), isZombie_(zombie), isDead_(health == 0) {}

    // методы
    bool isDead() { return isDead_; }
    int livePoints() { return health_; }
    void damage(int damage = 5) {
        if (isDead_) return;
        health_ -= damage;
        if (health_ <= 0) {
            isDead_ = true;
        }
    }
    void heal(int health = 3) {
        if (!isZombie_ && isDead_) return;
        health_ += health;
        if (health_ > 0) {
            isDead_ = false;
        }
    }
};