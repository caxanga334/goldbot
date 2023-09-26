#ifndef PLAYER_INPUT_H_
#define PLAYER_INPUT_H_

#include "sdk/timers.h"

class IPlayerInput
{
public:
	IPlayerInput()
	{
		m_buttons = 0;
		m_oldbuttons = 0;
		m_impulse = 0;
		m_forwardscale = 1.0f;
		m_sidescale = 1.0f;
	}

	/**
	 * @brief Resets and releases all buttons
	*/
	void Reset();
	/**
	 * @brief Call this to update the buttons that will be sent to the next runplayermove
	*/
	void Update();
	/**
	 * @brief Call this after sending the runplayermove to properly prepare the buttons for the next tick
	*/
	void EndUpdate() { m_buttons = 0; }

	void PressFireButton(const float duration = -1.0f);
	void ReleaseFireButton();
	void PressAltFireButton(const float duration = -1.0f);
	void ReleaseAltFireButton();
	void PressForwardButton(const float duration = -1.0f);
	void ReleaseForwardButton();
	void PressBackwardsButton(const float duration = -1.0f);
	void ReleaseBackwardsButton();
	void PressLeftButton(const float duration = -1.0f);
	void ReleaseLeftButton();
	void PressRightButton(const float duration = -1.0f);
	void ReleaseRightButton();
	void PressUseButton(const float duration = -1.0f);
	void ReleaseUseButton();
	void PressCrouchButton(const float duration = -1.0f);
	void ReleaseCrouchButton();
	void PressJumpButton(const float duration = -1.0f);
	void ReleaseJumpButton();
	void PressAltButton(const float duration = -1.0f);
	void ReleaseAltButton();
	void PressCancelButton(const float duration = -1.0f);
	void ReleaseCancelButton();
	void PressScoreButton(const float duration = -1.0f);
	void ReleaseScoreButton();
	void SetButtonScale(const float forward = 1.0f, const float side = 1.0f);
	int GetCurrentButtons() { return m_buttons; }
	int GetOldButtons() { return m_oldbuttons; }
	void SetImpulse(const int impulse) { m_impulse = impulse; }
	byte GetImpulse() { return static_cast<byte>(m_impulse); }
	/**
	 * @brief Gets the buttons that should be sent to runplayermove
	*/
	int GetInputButtons() { return m_buttons | m_oldbuttons; }
	float GetForwardScale() { return m_forwardscale; }
	float GetSideScale() { return m_sidescale; }
	/**
	 * @brief Checks if the forward and side movement speed should be scaled
	*/
	bool ShouldApplyScale() { return !m_scalebuttontimer.IsElapsed(); }

private:
	int m_buttons; // buttons that will be sent in the next usercmd
	int m_oldbuttons; // buttons that were pressed in the last usercmd sent
	int m_impulse; // impulse to be sent to the next usercmd
	float m_forwardscale; // forward velocity scale
	float m_sidescale; // side velocity scale

	CountdownTimer m_firebuttontimer;
	CountdownTimer m_altfirebuttontimer;
	CountdownTimer m_forwardbuttontimer;
	CountdownTimer m_backwardsbuttontimer;
	CountdownTimer m_leftbuttontimer;
	CountdownTimer m_rightbuttontimer;
	CountdownTimer m_usebuttontimer;
	CountdownTimer m_crouchbuttontimer;
	CountdownTimer m_jumpbuttontimer;
	CountdownTimer m_altbuttontimer;
	CountdownTimer m_cancelbuttontimer;
	CountdownTimer m_scorebuttontimer;
	CountdownTimer m_scalebuttontimer;
};

inline void IPlayerInput::Reset()
{
	m_buttons = 0;
	m_oldbuttons = 0;
	m_forwardscale = 1.0f;
	m_sidescale = 1.0f;
	m_firebuttontimer.Invalidate();
	m_altfirebuttontimer.Invalidate();
	m_forwardbuttontimer.Invalidate();
	m_backwardsbuttontimer.Invalidate();
	m_leftbuttontimer.Invalidate();
	m_rightbuttontimer.Invalidate();
	m_usebuttontimer.Invalidate();
	m_crouchbuttontimer.Invalidate();
	m_jumpbuttontimer.Invalidate();
	m_altbuttontimer.Invalidate();
	m_cancelbuttontimer.Invalidate();
	m_scorebuttontimer.Invalidate();
	m_scalebuttontimer.Invalidate();
}

inline void IPlayerInput::Update()
{
	if (!m_firebuttontimer.IsElapsed())
		m_buttons |= IN_ATTACK;

	if (!m_altfirebuttontimer.IsElapsed())
		m_buttons |= IN_ATTACK2;

	if (!m_forwardbuttontimer.IsElapsed())
		m_buttons |= IN_FORWARD;

	if (!m_backwardsbuttontimer.IsElapsed())
		m_buttons |= IN_BACK;

	if (!m_leftbuttontimer.IsElapsed())
		m_buttons |= IN_MOVELEFT;

	if (!m_rightbuttontimer.IsElapsed())
		m_buttons |= IN_MOVERIGHT;

	if (!m_usebuttontimer.IsElapsed())
		m_buttons |= IN_USE;

	if (!m_crouchbuttontimer.IsElapsed())
		m_buttons |= IN_DUCK;

	if (!m_jumpbuttontimer.IsElapsed())
		m_buttons |= IN_JUMP;

	if (!m_altbuttontimer.IsElapsed())
		m_buttons |= IN_ALT1;

	if (!m_cancelbuttontimer.IsElapsed())
		m_buttons |= IN_CANCEL;

	if (!m_scorebuttontimer.IsElapsed())
		m_buttons |= IN_SCORE;

	m_oldbuttons = m_buttons;
}

inline void IPlayerInput::PressFireButton(const float duration)
{
	m_buttons |= IN_ATTACK;
	m_firebuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseFireButton()
{
	m_buttons &= ~IN_ATTACK;
	m_firebuttontimer.Invalidate();
}

inline void IPlayerInput::PressAltFireButton(const float duration)
{
	m_buttons |= IN_ATTACK2;
	m_altfirebuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseAltFireButton()
{
	m_buttons &= ~IN_ATTACK2;
	m_altfirebuttontimer.Invalidate();
}

inline void IPlayerInput::PressForwardButton(const float duration)
{
	m_buttons |= IN_FORWARD;
	m_forwardbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseForwardButton()
{
	m_buttons &= ~IN_FORWARD;
	m_forwardbuttontimer.Invalidate();
}

inline void IPlayerInput::PressBackwardsButton(const float duration)
{
	m_buttons |= IN_BACK;
	m_backwardsbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseBackwardsButton()
{
	m_buttons &= ~IN_BACK;
	m_backwardsbuttontimer.Invalidate();
}

inline void IPlayerInput::PressLeftButton(const float duration)
{
	m_buttons |= IN_MOVELEFT;
	m_leftbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseLeftButton()
{
	m_buttons &= ~IN_MOVELEFT;
	m_leftbuttontimer.Invalidate();
}

inline void IPlayerInput::PressRightButton(const float duration)
{
	m_buttons |= IN_MOVERIGHT;
	m_rightbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseRightButton()
{
	m_buttons &= ~IN_MOVERIGHT;
	m_rightbuttontimer.Invalidate();
}

inline void IPlayerInput::PressUseButton(const float duration)
{
	m_buttons |= IN_USE;
	m_usebuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseUseButton()
{
	m_buttons &= ~IN_USE;
	m_usebuttontimer.Invalidate();
}

inline void IPlayerInput::PressCrouchButton(const float duration)
{
	m_buttons |= IN_DUCK;
	m_crouchbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseCrouchButton()
{
	m_buttons &= ~IN_DUCK;
	m_crouchbuttontimer.Invalidate();
}

inline void IPlayerInput::PressJumpButton(const float duration)
{
	m_buttons |= IN_JUMP;
	m_jumpbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseJumpButton()
{
	m_buttons &= ~IN_JUMP;
	m_jumpbuttontimer.Invalidate();
}

inline void IPlayerInput::PressAltButton(const float duration)
{
	m_buttons |= IN_ALT1;
	m_altbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseAltButton()
{
	m_buttons &= ~IN_ALT1;
	m_altbuttontimer.Invalidate();
}

inline void IPlayerInput::PressCancelButton(const float duration)
{
	m_buttons |= IN_CANCEL;
	m_cancelbuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseCancelButton()
{
	m_buttons &= ~IN_CANCEL;
	m_cancelbuttontimer.Invalidate();
}

inline void IPlayerInput::PressScoreButton(const float duration)
{
	m_buttons |= IN_SCORE;
	m_scorebuttontimer.Start(duration);
}

inline void IPlayerInput::ReleaseScoreButton()
{
	m_buttons &= ~IN_MOVELEFT;
	m_scorebuttontimer.Invalidate();
}

inline void IPlayerInput::SetButtonScale(const float forward, const float side)
{
	m_forwardscale = forward;
	m_sidescale = side;
	m_scalebuttontimer.Start(0.01f);
}

#endif // !PLAYER_INPUT_H_

