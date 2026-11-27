#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <string>

using namespace geode::prelude;

namespace {
    int stopParticles(CCNode* node, int depth) {
        if (!node || depth > 6) return 0;
        int count = 0;
        if (auto ps = typeinfo_cast<CCParticleSystem*>(node)) {
            ps->stopSystem();
            ps->setVisible(false);
            count++;
        }
        auto children = node->getChildren();
        if (!children) return count;
        CCObject* obj = nullptr;
        CCARRAY_FOREACH(children, obj) {
            count += stopParticles(static_cast<CCNode*>(obj), depth + 1);
        }
        return count;
    }
}

class $modify(LitePlayLayer, PlayLayer) {
    struct Fields {
        bool lite = false;
        int frames = 0;
        float elapsed = 0.f;
        int fps = 0;
        int hidden = 0;
        CCLabelBMFont* label = nullptr;
    };

    void updateLabel() {
        if (!m_fields->label) return;
        std::string s = m_fields->lite ? "Lite: ON" : "Lite: OFF";
        s += "  FPS " + std::to_string(m_fields->fps);
        if (m_fields->lite) {
            s += "  Particles hidden " + std::to_string(m_fields->hidden);
        }
        m_fields->label->setString(s.c_str());
    }

    void tickFrame(float dt) {
        m_fields->frames++;
        m_fields->elapsed += dt;
        if (m_fields->elapsed >= 1.f) {
            m_fields->fps = static_cast<int>(m_fields->frames / m_fields->elapsed + 0.5f);
            m_fields->frames = 0;
            m_fields->elapsed = 0.f;
            updateLabel();
        }
    }

    void tickSlow(float) {
        if (!m_fields->lite) return;
        m_fields->hidden = stopParticles(this, 0);
    }

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto menu = CCMenu::create();
        menu->setPosition(ccp(0.f, 0.f));
        auto spr = ButtonSprite::create("Lite");
        spr->setScale(0.6f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(LitePlayLayer::onToggleLite));
        btn->setPosition(ccp(40.f, 40.f));
        menu->addChild(btn);
        this->addChild(menu, 1000);

        m_fields->label = CCLabelBMFont::create("Lite: OFF", "bigFont.fnt");
        m_fields->label->setScale(0.3f);
        m_fields->label->setAnchorPoint(ccp(0.f, 0.5f));
        m_fields->label->setPosition(ccp(75.f, 40.f));
        this->addChild(m_fields->label, 1000);

        this->schedule(schedule_selector(LitePlayLayer::tickFrame));
        this->schedule(schedule_selector(LitePlayLayer::tickSlow), 0.5f);
        updateLabel();
    }

    void onToggleLite(CCObject*) {
        m_fields->lite = !m_fields->lite;
        if (m_fields->lite) {
            m_fields->hidden = stopParticles(this, 0);
        } else {
            m_fields->hidden = 0;
        }
        updateLabel();
    }
};
