#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <algorithm>

using namespace geode::prelude;

class MapOverlay : public CCLayerColor {
protected:
    CCSprite* m_image = nullptr;
    CCLabelBMFont* m_close = nullptr;
    CCPoint m_dragOffset;
    bool m_dragging = false;

    bool init() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        if (!CCLayerColor::initWithColor(
            {0, 0, 0, 180},
            winSize.width,
            winSize.height
        )) {
            return false;
        }

        this->setTouchEnabled(true);
        this->setTouchMode(kCCTouchesOneByOne);

        auto imagePath =
            (Mod::get()->getResourcesDir() / "imagem.png").string();

        m_image = CCSprite::create(imagePath.c_str());

        if (!m_image) {
            log::error("THE map: nao foi possivel carregar imagem.png");
            return false;
        }

        float scaleX = winSize.width * 0.85f / m_image->getContentSize().width;
        float scaleY = winSize.height * 0.85f / m_image->getContentSize().height;

        m_image->setScale(std::min(scaleX, scaleY));
        m_image->setPosition(
            {winSize.width / 2, winSize.height / 2}
        );

        this->addChild(m_image, 1);

        m_close = CCLabelBMFont::create("X", "bigFont.fnt");
        m_close->setScale(0.8f);
        m_close->setPosition({
            winSize.width - 35,
            winSize.height - 35
        });

        this->addChild(m_close, 2);

        return true;
    }

public:
    static MapOverlay* create() {
        auto ret = new MapOverlay();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::sharedDispatcher()
            ->addTargetedDelegate(this, -500, true);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        auto location = this->convertToNodeSpace(touch->getLocation());

        if (m_close->boundingBox().containsPoint(location)) {
            this->removeFromParentAndCleanup(true);
            return true;
        }

        if (m_image->boundingBox().containsPoint(location)) {
            m_dragging = true;
            m_dragOffset = m_image->getPosition() - location;
        }

        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (!m_dragging) {
            return;
        }

        auto location = this->convertToNodeSpace(touch->getLocation());
        m_image->setPosition(location + m_dragOffset);
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        m_dragging = false;
    }

    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override {
        m_dragging = false;
    }
};

#include <Geode/modify/MenuLayer.hpp>

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto menu = this->getChildByID("bottom-menu");

        if (!menu) {
            return true;
        }

        auto button = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
            this,
            menu_selector(MyMenuLayer::onOpenMap)
        );

        if (button) {
            button->setID("the-map-button"_spr);
            menu->addChild(button);
            menu->updateLayout();
        }

        return true;
    }

    void onOpenMap(CCObject*) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();

        if (!scene) {
            return;
        }

        auto overlay = MapOverlay::create();

        if (overlay) {
            scene->addChild(overlay, 999999);
        }
    }
};
