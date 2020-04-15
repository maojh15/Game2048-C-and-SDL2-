-- configuration file
-- source files
imgSrcFilename = "src/2048_.png"
newGameButtonImageFilename = "src/newGameButton.png"
tryagainButtonImgFilename = "src/tryagainBtn.png"
gameoverImgFilename = "src/gameover.png"


-- set various parameters
frame = 40 --FPS
scr_width = 800
scr_height = 700

-- set UI
-- position of button
newGameButtonPos = {x = scr_width * 0.6,
                    y = scr_height * 0.058}
gameoverImgCenterPos = {x = scr_width / 2,
                        y = scr_height * 0.6};
tryagainBtnCenterPos = {x = scr_width / 2,
                        y = scr_height * 3 / 4};
