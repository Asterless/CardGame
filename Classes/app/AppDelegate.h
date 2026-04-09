/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef CARDGAME_APP_DELEGATE_H
#define CARDGAME_APP_DELEGATE_H

#include "cocos2d.h"

/**
@brief cocos2d 应用入口。

这里使用私有继承，避免直接暴露完整的 Application 接口。
*/
class AppDelegate : public cocos2d::Application
{
public:
    AppDelegate();
    ~AppDelegate() override;

    void initGLContextAttrs() override;

    /**
    @brief 在这里完成 Director 和主场景初始化。
    @return true 初始化成功，应用继续运行。
    @return false 初始化失败，应用终止。
    */
    bool applicationDidFinishLaunching() override;

    /**
    @brief 应用切到后台时调用。
    @param the pointer of the application
    */
    void applicationDidEnterBackground() override;

    /**
    @brief 应用回到前台时调用。
    @param the pointer of the application
    */
    void applicationWillEnterForeground() override;
};

#endif // CARDGAME_APP_DELEGATE_H
