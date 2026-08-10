/*
 * KegScale.h
 *
 *  Created on: Jan 3, 2021 !
 *      Author: Mike
 */

#ifndef KEGSCALEOBJ_H_
#define KEGSCALEOBJ_H_
#pragma once

#include <Arduino.h>
#include <HX711.h>

class KegScaleObj: public HX711 {
	private:
		int kegNumber = 0;
		long fullCount = 0;
		long emptyCount = 0;
		long scaleCount = 0;
		double gallons = 0;
		double percentFull = 0;
		long glassesLeft = 0;
		int dataPin = 0;
		int clockPin = 0;
		String kegEmptyCountMQTTTag;
		String kegFullCountMQTTTag;
		String scaleCountMQTTTag;
		String kegGallonsMQTTTag;
		String kegPercentFullMQTTTag;
		String kegGlassesLeftMQTTTag;
		String scaleCalibrateEmptyMQTTTag;
		String scaleCalibrateFullMQTTTag;

	public:
		KegScaleObj();

		virtual ~KegScaleObj();

		void init(int keg);
		void setKegNumber(int keg);
		int getKegNumber();
		void setFullCount(long count);
		long getFullCount();
		void setEmptyCount(long count);
		long getEmptyCount();
		void setScaleCount(long count);
		long getScaleCount();
		void readScale(int samples, int retries, unsigned long delay_ms);
		double getGallons();
		double getPercentFull();
		long getGlassesLeft();
		void setKegEmptyCountMQTTTag(String tag);
		String getKegEmptyCountMQTTTag();
		void setKegFullCountMQTTTag(String tag);
		String getKegFullCountMQTTTag();
		void setScaleCountMQTTTag(String tag);
		String getScaleCountMQTTTag();
		void setKegGallonsMQTTTag(String tag);
		String getKegGallonsMQTTTag();
		void setKegPercentFullMQTTTag(String tag);
		String getKegPercentFullMQTTTag();
		void setKegGlassesLeftMQTTTag(String tag);
		String getKegGlassesLeftMQTTTag();
		void setScaleCalibrateEmptyMQTTTag(String tag);
		String getScaleCalibrateEmptyMQTTTag();
		void setScaleCalibrateFullMQTTTag(String tag);
		String getScaleCalibrateFullMQTTTag();
		void calibrateEmpty(int samples, int retries, unsigned long delay_ms);
		void calibrateFull(int samples, int retries, unsigned long delay_ms);

};

#endif /* KEGSCALEOBJ_H_ */
