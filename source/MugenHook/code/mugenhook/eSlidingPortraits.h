#pragma once

enum eAxis {
	AXIS_X,
	// todo
	AXIS_Y,
	AXIS_BOTH
};

class eSlidingPortraits {
public:
	static int m_tTimer;
	static int m_tTimerP2;
	static float p1_portrait_slide_speed;
	static float p2_portrait_slide_speed;
	static float p1_portrait_slide_max_dist;
	static float p2_portrait_slide_max_dist;
	static int   p1_portrait_axis;
	static int   p2_portrait_axis;
	static int   portrait_slide_reset_on_move;
	static bool m_bStopSliding;
	static bool	m_bStopSlidingP2;

	static void Init();
	static void Process();
	static void ProcessP2();
	static void Reset();
	static void ResetPlayer(int player);
};