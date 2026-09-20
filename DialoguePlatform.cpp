#include "DialoguePlatform.h"

DialoguePlatform::DialoguePlatform(shared_ptr<TextureManager>textureManager):
	textureManager(textureManager)
{
	squareFrame = this->textureManager->Asset("square_frame");
	horizontalFrame = this->textureManager->Asset("horizontal_frame");

	rightPosX = 0;
	rightPosY = 0;

	portraitWidth = DIALOGUE_PORTRAIT_WIDTH;
	portraitHeight = DIALOGUE_PORTRAIT_HEIGHT;

	leftPosX = SCR_WIDTH - portraitWidth;
	leftPosY = 0;

	

    rightSquareFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(squareFrame->width), static_cast<float>(squareFrame->height) };
    rightSquareFrameDestRect = Rectangle{ static_cast<float>(rightPosX), static_cast<float>(rightPosY), static_cast<float>(portraitWidth), static_cast<float>(portraitHeight) };

    leftSquareFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(squareFrame->width), static_cast<float>(squareFrame->height) };
    leftSquareFrameDestRect = Rectangle{ static_cast<float>(leftPosX), static_cast<float>(leftPosY), static_cast<float>(portraitWidth), static_cast<float>(portraitHeight) };


    horizontalFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(horizontalFrame->width), static_cast<float>(horizontalFrame->height) };
    //horizontalFrameDestRect = Rectangle{ static_cast<float>(portraitWidth), 0.0f, static_cast<float>(SCR_WIDTH - portraitWidth * 2), static_cast<float>(portraitHeight) };
	horizontalFrameDestRect = Rectangle{ 0.0f, static_cast<float>(SCR_HEIGHT - (SCR_HEIGHT/3)), static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT / 3) };


	origin = { 0.0f, 0.0f };
}

void DialoguePlatform::SetDialoguePlatform(Texture2D* rightPortrait, Texture2D* leftPortrait, vector<Button>* buttons)
{

	//cout << "SET DIALOGUE PLATFORM" << endl;
	rightPortraitSource = {
		0.0f,
		0.0f,
		static_cast<float>(rightPortrait->width),
		static_cast<float>(rightPortrait->height)
	};

	rightPortraitDest = {
		static_cast<float>(rightPosX),
		static_cast<float>(rightPosY),
		-static_cast<float>(portraitWidth),
		static_cast<float>(portraitHeight)
	};

	leftPortraitSource = {
		0.0f,
		0.0f,
		static_cast<float>(leftPortrait->width),
		static_cast<float>(leftPortrait->height)
	};

	leftPortraitDest = {
		static_cast<float>(leftPosX),
		static_cast<float>(leftPosY),
		static_cast<float>(portraitWidth),
		static_cast<float>(portraitHeight)
	};

	this->buttons = buttons;

	if (buttons != nullptr)
		AdjustButtons();
	else
	{
		cout << "buttons are null\n";
	}
}

void DialoguePlatform::SetDialoguePlatform(MediaStream* rightPortrait, MediaStream* leftPortrait, vector<Button>* buttons)
{

	// STORE THEM
	this->rightPortrait = rightPortrait;
	this->leftPortrait = leftPortrait;

	// SAFETY CHECK
	if (!rightPortrait || !leftPortrait) return;
	cout << "Portraits are not null\n";

	if (IsMediaValid(*rightPortrait))
	{
		SetMediaState(*rightPortrait, MEDIA_STATE_PLAYING);
		// optional: restart from the beginning
		// SetMediaPosition(*rightPortrait, 0.0);
	}

	if (IsMediaValid(*leftPortrait))
	{
		SetMediaState(*leftPortrait, MEDIA_STATE_PLAYING);
		// SetMediaPosition(*leftPortrait, 0.0);
	}

	float rightScale = std::min(
		480.0f / (float) this->rightPortrait->videoTexture.width,
		720.0f / (float) this->rightPortrait->videoTexture.height
	);

	float leftScale = std::min(
		480.0f / (float)this->leftPortrait->videoTexture.width,
		720.0f / (float)this->leftPortrait->videoTexture.height
	);


	//cout << "SET DIALOGUE PLATFORM" << endl;
	rightPortraitSource = {
		0.0f,
		0.0f,
		static_cast<float>(rightPortrait->videoTexture.width),
		static_cast<float>(rightPortrait->videoTexture.height)
	};

	rightPortraitDest = {
		static_cast<float>(rightPosX),
		static_cast<float>(rightPosY),
		-static_cast<float>(this->rightPortrait->videoTexture.width * rightScale),
		static_cast<float>(this->rightPortrait->videoTexture.height * rightScale)
	};

	leftPortraitSource = {
		0.0f,
		0.0f,
		static_cast<float>(leftPortrait->videoTexture.width),
		static_cast<float>(leftPortrait->videoTexture.height)
	};

	leftPosX = SCR_WIDTH - this->leftPortrait->videoTexture.width * leftScale;

	leftPortraitDest = {
		static_cast<float>(leftPosX),
		static_cast<float>(leftPosY),
		static_cast<float>(this->leftPortrait->videoTexture.width * leftScale),
		static_cast<float>(this->leftPortrait->videoTexture.height * leftScale)
	};

	this->buttons = buttons;

	if (buttons != nullptr)
		AdjustButtons();
	else
	{
		cout << "buttons are null\n";
	}
}

void DialoguePlatform::DrawPlatform(Texture2D* rightPortrait, Texture2D* leftPortrait, const string& name, const vector<TextBox>& dialogue, int dialogueCounter, float dt, bool animated)
{
	if (dialogueCounter <= 0) return;

	// Background behind the RIGHT portrait
	DrawRectangle(rightPosX, rightPosY, rightPortraitDest.width, rightPortraitDest.height, INTERFACE_BG_COLOR);
	// Background behind the LEFT portrait
	DrawRectangle(leftPosX, leftPosY, leftPortraitDest.width, leftPortraitDest.height, INTERFACE_BG_COLOR);
	//// Background behind horizontal
	//DrawRectangle(portraitWidth, 0, horizontalFrameDestRect.width, horizontalFrameDestRect.height, INTERFACE_BG_COLOR);  



	if (animated)
	{
		//// Display RIGHT portrait
		//DrawAnimatedPortrait(rightPortrait, rightAnimatedPortraitSource, rightPortraitDest, dt, rightPortraitFrameTimer, rightPortraitFrameIndex);
		//// Display LEFT portrait
		//DrawAnimatedPortrait(leftPortrait, leftAnimatedPortraitSource, leftPortraitDest, dt, leftPortraitFrameTimer, leftPortraitFrameIndex);

		if (this->rightPortrait && this->rightPortrait->videoTexture.id != 0)
		{
			DrawTexturePro(
				this->rightPortrait->videoTexture,
				rightPortraitSource,
				rightPortraitDest,
				{ 0, 0 },
				0.0f,
				WHITE
			);
		}

		if (this->leftPortrait && this->leftPortrait->videoTexture.id != 0)
		{
			DrawTexturePro(
				this->leftPortrait->videoTexture,
				leftPortraitSource,
				leftPortraitDest,
				{ 0, 0 },
				0.0f,
				WHITE
			);
		}
		else
		{
			cout << "error" << endl;
		}
	}
	else
	{

		// Display RIGHT portrait
		DrawTexturePro(*rightPortrait, rightPortraitSource, rightPortraitDest, origin, 0.0f, RAYWHITE);

		// Display LEFT portrait
		//DrawTexturePro(*leftPortrait, leftPortraitSource, leftPortraitDest, origin, 0.0f, RAYWHITE);
		DrawAnimatedPortrait(leftPortrait, leftAnimatedPortraitSource, leftPortraitDest, dt, leftPortraitFrameTimer, leftPortraitFrameIndex);
	}
	// Display RIGHT portrait frame
	DrawTexturePro(*squareFrame, rightSquareFrameSourceRect, rightPortraitDest, origin, 0.0f, RAYWHITE);

	// Display LEFT portrait frame
	DrawTexturePro(*squareFrame, leftSquareFrameSourceRect, leftPortraitDest, origin, 0.0f, RAYWHITE);

	// Display horizontal frame
	
	DrawRectangleRec(horizontalFrameDestRect, INTERFACE_BG_COLOR); // Background behind horizontal frame
	DrawTexturePro(*horizontalFrame, horizontalFrameSourceRect, horizontalFrameDestRect, origin, 0.0f, RAYWHITE);
	//DrawText((name + ":").c_str(), portraitWidth + 75, rightPortraitDest.height / 3, 25, Color(253, 221, 0, 255));
	DrawText((name + ":").c_str(), horizontalFrameDestRect.x + 75, horizontalFrameDestRect.y + 50, 25, Color(253, 221, 0, 255));
	//DrawText(dialogue[dialogueCounter].GetText().c_str(), portraitWidth + 75, rightPortraitDest.height / 2, 20, RAYWHITE);
	DrawText(dialogue[dialogueCounter].GetText().c_str(), horizontalFrameDestRect.x + 75, horizontalFrameDestRect.y + 75, 20, RAYWHITE);
	
	if(buttons != nullptr)
		for (Button& button : *buttons)
			button.Display();

}

void DialoguePlatform::SetButtons(vector<Button>& buttons)
{
	cout << "SET BUTTONS\n";
	if (buttons.size() > 0)
	{
		this->buttons = &buttons;
	}
	else
		this->buttons = nullptr;
}

void DialoguePlatform::AdjustButtons()
{
	if (buttons == nullptr) return;
	cout << "ADJUST BUTTONS\n";
	int offsetX = 0;
	for (Button& button : *buttons)
	{
		button.SetPosition(horizontalFrameDestRect.x + leftSquareFrameDestRect.width + offsetX, horizontalFrameDestRect.y - button.GetRect().height);

		//button.SetPosition(SCR_WIDTH / 2 + offsetX, SCR_HEIGHT / 2);
		offsetX += button.GetRect().width;
	}
}


void DialoguePlatform::DrawAnimatedPortrait(
	Texture2D* portrait,
	Rectangle& portraitSource,
	Rectangle portraitDest,
	float dt,
	float& portraitFrameTimer,
	int& portraitFrameIndex
)
{
	// Update animation timer
	portraitFrameTimer += dt;
	if (portraitFrameTimer >= (1.0f / animSpeed)) {
		portraitFrameTimer = 0.0f;

		portraitFrameIndex++;
		if (portraitFrameIndex >= totalPortraitFrames)
			portraitFrameIndex = 0;

		int col = portraitFrameIndex % animPortCols;
		int row = portraitFrameIndex / animPortCols;

		// UPDATE THE REAL portraitSource
		portraitSource.x = static_cast<float>(col * animPortraitWidth);
		portraitSource.y = static_cast<float>(row * animPortraitHeight);
	}

	DrawTexturePro(
		*portrait,
		portraitSource,
		portraitDest,
		origin,
		0.0f,
		RAYWHITE
	);
}

void DialoguePlatform::Update()
{
	if (rightPortrait && rightPortrait->videoTexture.id != 0 &&
		GetMediaState(*rightPortrait) == MEDIA_STATE_PLAYING)
	{
		UpdateMedia(rightPortrait);
	}

	if (leftPortrait && leftPortrait->videoTexture.id != 0 &&
		GetMediaState(*leftPortrait) == MEDIA_STATE_PLAYING)
	{
		UpdateMedia(leftPortrait);
	}
}

void DialoguePlatform::ResetPortraits()
{
	rightPortrait = nullptr;
	leftPortrait = nullptr;
}