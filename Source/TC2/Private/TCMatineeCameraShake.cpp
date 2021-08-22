// Fill out your copyright notice in the Description page of Project Settings.


#include "TCMatineeCameraShake.h"

UTCMatineeCameraShake::UTCMatineeCameraShake() : Super()
{
    OscillationDuration = 0.2;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Pitch.Amplitude = 1;
	RotOscillation.Pitch.Frequency = 15;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::PerlinNoise;

	RotOscillation.Yaw.Amplitude = 1;
	RotOscillation.Yaw.Frequency = 15;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::PerlinNoise;
}
