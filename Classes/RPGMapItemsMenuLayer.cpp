//
//  RPGMapItemsMenuLayer.cpp
//  OzgGameRPG
//
//  Created by ozg on 14-4-10.
//
//

#include "RPGMapItemsMenuLayer.h"
#include "RPGMapMenuLayer.h"
#include "RPGMapSceneLayer.h"
#include "RPGResultsLogic.h"

RPGMapItemsMenuLayer::RPGMapItemsMenuLayer()
{

}

RPGMapItemsMenuLayer::~RPGMapItemsMenuLayer()
{
    this->m_stringList->release();
    this->m_itemsList->release();
    
    CCLog("RPGMapItemsMenuLayer 释放");
}

bool RPGMapItemsMenuLayer::init(cocos2d::CCDictionary *stringList, CppSQLite3DB *db, float width, float height)
{
    if(CCLayerColor::initWithColor(ccc4(0, 0, 0, 200), width, height))
    {
        this->m_stringList = stringList;
        this->m_stringList->retain();
        
        this->m_db = db;
        
        this->m_itemsList = new CCArray();
        this->m_itemsList->init();
        
        CCTMXTiledMap *mainBg = CCTMXTiledMap::create(CCString::createWithFormat("map_menu3_%s.tmx", CCUserDefault::sharedUserDefault()->getStringForKey(GAME_STYLE).c_str())->getCString());
        mainBg->setPosition(CCPointZero);
        mainBg->setTag(kRPGMapItemsMenuLayerTagBg);
        this->addChild(mainBg);
        
        CCMenu *mainMenu = CCMenu::create();
        mainMenu->setTag(kRPGMapItemsMenuLayerTagMainMenu);
        mainMenu->setAnchorPoint(CCPointZero);
        mainMenu->setPosition(CCPointZero);
        this->addChild(mainMenu);
        
        CCMenuItemSprite *menuBack = (CCMenuItemSprite*)mainMenu->getChildByTag(kRPGMapItemsMenuLayerTagMainMenuBack);
        if(!menuBack)
        {
            menuBack = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("commons_btn_back_04.png"), CCSprite::createWithSpriteFrameName("commons_btn_back_04.png"), this, menu_selector(RPGMapItemsMenuLayer::onMenu));
            menuBack->setPosition(ccp(40, 600));
            menuBack->setTag(kRPGMapItemsMenuLayerTagMainMenuBack);
            menuBack->setScale(0.5);
            mainMenu->addChild(menuBack);
        }
        
        CCTableView *tableView = CCTableView::create(this, CCSizeMake(900, 570));
        tableView->setDirection(kCCScrollViewDirectionVertical);
        tableView->setPosition(ccp(80, 30));
        tableView->setDelegate(this);
        tableView->setVerticalFillOrder(kCCTableViewFillTopDown);
        tableView->setTag(kRPGMapItemsMenuLayerTagItemListTable);
        this->addChild(tableView);
        
        this->loadItemsData();
        
        return true;
    }
    return false;
}

RPGMapItemsMenuLayer* RPGMapItemsMenuLayer::create(cocos2d::CCDictionary *stringList, CppSQLite3DB *db, float width, float height)
{
    RPGMapItemsMenuLayer *obj = new RPGMapItemsMenuLayer();
    if(obj && obj->init(stringList, db, width, height))
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return NULL;
}

//CCTableView
void RPGMapItemsMenuLayer::scrollViewDidScroll(CCScrollView *scrollView)
{
    
}

void RPGMapItemsMenuLayer::scrollViewDidZoom(CCScrollView *scrollView)
{
    
}

void RPGMapItemsMenuLayer::tableCellTouched(CCTableView *tableView, CCTableViewCell *cell)
{
    
}

CCSize RPGMapItemsMenuLayer::cellSizeForTable(CCTableView *tableView)
{
    return CCSizeMake(tableView->getContentSize().width, 80);
}

CCTableViewCell* RPGMapItemsMenuLayer::tableCellAtIndex(CCTableView *tableView, unsigned int idx)
{
    CCTableViewCell *cell = tableView->dequeueCell();
    if (!cell)
    {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    else
        cell->removeAllChildrenWithCleanup(true);
    
    float x = 100;
    for (int i = 0; i < 4; i++)
    {
        int index = idx * 4 + i;
        
        if(index >= this->m_itemsList->count())
            break;
        
        RPGExistingItems *itemsData = (RPGExistingItems*)this->m_itemsList->objectAtIndex(index);
        
        CCControlButton *itemBtn = CCControlButton::create(CCString::createWithFormat("%s (%i)", itemsData->m_name.c_str(), itemsData->m_total)->getCString(), "Arial", 22);
        itemBtn->setPosition(ccp(x, 0));
        itemBtn->setTag(itemsData->m_dataId);
        itemBtn->addTargetWithActionForControlEvents(this, cccontrol_selector(RPGMapItemsMenuLayer::onButton), CCControlEventTouchUpInside);
        cell->addChild(itemBtn);
        
        x += 200;
    }
    
    return cell;
}

unsigned int RPGMapItemsMenuLayer::numberOfCellsInTableView(CCTableView *tableView)
{
    int rowItemCount = 4;
    int size = this->m_itemsList->count();
    
    if(size % rowItemCount == 0)
        return size / rowItemCount;
    else
        return size / rowItemCount + 1;
}
//CCTableView end

//private
void RPGMapItemsMenuLayer::loadItemsData()
{
    //道具数据
    this->m_itemsList->removeAllObjects();
    
    CppSQLite3Query query = this->m_db->execQuery(ITEMS_EXISTING_QUERY);
    while(!query.eof())
    {
        RPGExistingItems *itemsData = RPGExistingItems::create();
        itemsData->m_dataId = query.getIntField("id");
        itemsData->m_name = query.getStringField("name_cns");
        itemsData->m_buy = query.getIntField("buy");
        itemsData->m_sell = query.getIntField("sell");
        itemsData->m_type = query.getIntField("type");
        itemsData->m_attack = query.getFloatField("attack");
        itemsData->m_defense = query.getFloatField("defense");
        itemsData->m_speed = query.getFloatField("speed");
        itemsData->m_skillAttack = query.getFloatField("skill_attack");
        itemsData->m_skillDefense = query.getFloatField("skill_defense");
        itemsData->m_total = query.getIntField("total");
        this->m_itemsList->addObject(itemsData);
        
        query.nextRow();
    }
    query.finalize();
    
    CCTableView *tableView = (CCTableView*)this->getChildByTag(kRPGMapItemsMenuLayerTagItemListTable);
    tableView->reloadData();
}

void RPGMapItemsMenuLayer::onMenu(cocos2d::CCObject *pObject)
{
    CCMenuItem *menuItem = (CCMenuItem*)pObject;
    SimpleAudioEngine::sharedEngine()->playEffect("audio_effect_btn.wav");
    
    switch (menuItem->getTag())
    {
        case kRPGMapItemsMenuLayerTagMainMenuBack:
        {
//            CCLog("后退");
            
            //因为动态获取地图的大小会导致了菜单层显示错位，所以定死了
            float width = 960;
            float height = 640;
            RPGMapMenuLayer *menuLayer = menuLayer = RPGMapMenuLayer::create(this->m_stringList, this->m_db, width, height);
            menuLayer->setPosition(ccp((CCDirector::sharedDirector()->getWinSize().width - width) / 2.0, (CCDirector::sharedDirector()->getWinSize().height - height) / 2.0));
            menuLayer->setTag(kRPGMapSceneLayerTagMenuLayer);
            this->getParent()->addChild(menuLayer);
            
            this->removeFromParentAndCleanup(true);
            
        }
            break;
            
        default:
            break;
    }
    
}

void RPGMapItemsMenuLayer::onDialog(cocos2d::CCObject *pObject)
{
    this->getParent()->removeChildByTag(kRPGMapItemsMenuLayerTagDialog, true);
    this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayerBg, true);
    this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayer, true);
    this->getParent()->removeChildByTag(kRPGMapItemsMenuLayerTagBtnDiscard, true);
    this->setVisible(true);
}

void RPGMapItemsMenuLayer::onButton(cocos2d::CCObject *pSender, CCControlEvent event)
{
    SimpleAudioEngine::sharedEngine()->playEffect("audio_effect_btn.wav");
    
    CCControlButton *itemBtn = (CCControlButton*)pSender;
    
    if(itemBtn->getTag() == kRPGMapItemsMenuLayerTagBtnDiscard)
    {
//        CCLog("丢弃道具");
        
        RPGResultsLogic::discardItems(this->m_db, this->m_selectedItems->m_dataId);
        
        this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayerBg, true);
        this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayer, true);
        this->getParent()->removeChildByTag(kRPGMapItemsMenuLayerTagBtnDiscard, true);
        this->setVisible(true);
        
        this->loadItemsData();
    }
    else
    {
        for (int i = 0; i < this->m_itemsList->count(); i++)
        {
            //判断选中道具
            RPGExistingItems *itemsData = (RPGExistingItems*)this->m_itemsList->objectAtIndex(i);
            if(itemBtn->getTag() == itemsData->m_dataId)
            {
                this->m_selectedItems = itemsData;
                break;
            }
        }
        
        //因为动态获取地图的大小会导致了菜单层显示错位，所以定死了
        float width = 960;
        float height = 640;
        
        //临时背景
        CCTMXTiledMap *mainBg = CCTMXTiledMap::create(CCString::createWithFormat("map_menu3_%s.tmx", CCUserDefault::sharedUserDefault()->getStringForKey(GAME_STYLE).c_str())->getCString());
        mainBg->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2));
        mainBg->setAnchorPoint(ccp(0.5, 0.5));
        mainBg->setTag(kRPGMapSceneLayerTagChoicePlayerMenuLayerBg);
        this->getParent()->addChild(mainBg);
        
        CCString *title = CCString::createWithFormat(((CCString*)this->m_stringList->objectForKey("menu_items_choice"))->getCString(), this->m_selectedItems->m_name.c_str());
        RPGMapChoicePlayerMenuLayer *choicePlayer = RPGMapChoicePlayerMenuLayer::create(this->m_db, title, this, callfuncO_selector(RPGMapItemsMenuLayer::onChoicePlayer), width, height);
        choicePlayer->setTag(kRPGMapSceneLayerTagChoicePlayerMenuLayer);
        this->getParent()->addChild(choicePlayer);
        
        if(this->m_selectedItems->m_type != 3)
        {
//            CCLog("非道具不可以使用");
            choicePlayer->setHidden(true);
        }
        
        CCString *btnDiscardText = CCString::createWithFormat(((CCString*)this->m_stringList->objectForKey("menu_items_discard"))->getCString(), this->m_selectedItems->m_name.c_str());
        CCControlButton *btnDiscard = CCControlButton::create(btnDiscardText->getCString(), "Arial", 24);
        btnDiscard->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2, 250));
        btnDiscard->setTag(kRPGMapItemsMenuLayerTagBtnDiscard);
        btnDiscard->addTargetWithActionForControlEvents(this, cccontrol_selector(RPGMapItemsMenuLayer::onButton), CCControlEventTouchUpInside);
        this->getParent()->addChild(btnDiscard);
        
        this->setVisible(false);
    }
    
}

void RPGMapItemsMenuLayer::onChoicePlayer(cocos2d::CCObject *pObject)
{
    SimpleAudioEngine::sharedEngine()->playEffect("audio_effect_btn.wav");
    
    CCMenuItem *menuItem = (CCMenuItem*)pObject;
    if(menuItem->getTag() == kRPGMapChoicePlayerMenuLayerTagMainMenuBack)
    {
        //按了退出
        this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayerBg, true);
        this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayer, true);
        this->getParent()->removeChildByTag(kRPGMapItemsMenuLayerTagBtnDiscard, true);
        this->setVisible(true);
    }
    else
    {
        bool isSuccess = RPGResultsLogic::useItems(this->m_db, menuItem->getTag() - kRPGMapChoicePlayerMenuLayerTagMainMenuPlayer, this->m_selectedItems->m_dataId);
        if(!isSuccess)
        {
            //道具使用失败
            CCSize winSize = CCDirector::sharedDirector()->getWinSize();
            RPGDialogLayer *dialog = RPGDialogLayer::create(((CCString*)this->m_stringList->objectForKey("menu_items_fail"))->getCString(), ((CCString*)this->m_stringList->objectForKey("confirm_exit_ok"))->getCString(), kRPGMapItemsMenuLayerTagDialogOK, winSize.width, winSize.height, this, menu_selector(RPGMapItemsMenuLayer::onDialog));
            dialog->setTag(kRPGMapItemsMenuLayerTagDialog);
            this->getParent()->addChild(dialog);
            
        }
        else
        {
            //道具使用成功
            this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayerBg, true);
            this->getParent()->removeChildByTag(kRPGMapSceneLayerTagChoicePlayerMenuLayer, true);
            this->getParent()->removeChildByTag(kRPGMapItemsMenuLayerTagBtnDiscard, true);
            this->setVisible(true);
            
            this->loadItemsData();
        }
    }
    
}
