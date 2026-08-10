/*
 * KegScale.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: Mike
 */

#include "Arduino.h"
#include "KegScaleObj.h"
#include "config.h"


KegScaleObj::KegScaleObj() {
}

KegScaleObj::~KegScaleObj() {
}

void KegScaleObj::init(int index) {
	setKegNumber(index+1);

	//Setup MQTT Tags that will be used for each keg.
	setKegEmptyCountMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/sp/emptycount");
	setKegFullCountMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/sp/fullcount");
	setScaleCountMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/pv/scalecount");
	setKegGallonsMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/pv/gallons");
	setKegPercentFullMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/pv/percentfull");
	setKegGlassesLeftMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/pv/glassesleft");
	setScaleCalibrateEmptyMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/cmd/calibrateempty");
	setScaleCalibrateFullMQTTTag("home/keezer/kegscale/keg" + String(kegNumber) + "/cmd/calibratefull");

	//Set the microcontroller pins
	begin(KEG_PINS[index].dataPin, KEG_PINS[index].clockPin, 128);
	set_scale();
	power_down();
}

void KegScaleObj::readScale(int samples = 10, int retries = 10, unsigned long delay_ms = 100) {
	power_up();
	if (wait_ready_retry(retries, delay_ms)) {
		scaleCount = read_average(samples);
		if ((fullCount-emptyCount) == 0){
			percentFull =  0.0;
		} else {
		    percentFull = (double)(scaleCount-emptyCount)/(double)(fullCount-emptyCount);
		}
		gallons = percentFull * 5.0;
		glassesLeft = (long)(gallons * 128.0 / 12.0);
	} else {
		scaleCount = 0;              // set to zero for error condition
		gallons = 0.0;
		percentFull = 0.0;
		glassesLeft = 0;
	}
	power_down();
}


void KegScaleObj::setKegNumber(int keg) {
	kegNumber = keg;
}

int KegScaleObj::getKegNumber() {
	return kegNumber;
}

void KegScaleObj::setScaleCount(long count) {
	scaleCount = count;
}

long KegScaleObj::getScaleCount() {
	return scaleCount;
}

void KegScaleObj::setFullCount(long count) {
	fullCount = count;
}

long KegScaleObj::getFullCount() {
	return fullCount;
}

void KegScaleObj::setEmptyCount(long count) {
	emptyCount = count;
}

long KegScaleObj::getEmptyCount() {
	return emptyCount;
}

double KegScaleObj::getGallons() {
	return gallons;
}

double KegScaleObj::getPercentFull() {
	return percentFull;
}

long KegScaleObj::getGlassesLeft() {
	return glassesLeft;
}

void KegScaleObj::setKegEmptyCountMQTTTag(String tag) {
	kegEmptyCountMQTTTag = tag;
}

String KegScaleObj::getKegEmptyCountMQTTTag() {
	return kegEmptyCountMQTTTag;
}

void KegScaleObj::setKegFullCountMQTTTag(String tag) {
	kegFullCountMQTTTag = tag;
}

String KegScaleObj::getKegFullCountMQTTTag() {
	return kegFullCountMQTTTag;
}

void KegScaleObj::setScaleCountMQTTTag(String tag) {
	scaleCountMQTTTag = tag;
}

String KegScaleObj::getScaleCountMQTTTag() {
	return scaleCountMQTTTag;
}

void KegScaleObj::setKegGallonsMQTTTag(String tag) {
	kegGallonsMQTTTag = tag;
}

String KegScaleObj::getKegGallonsMQTTTag() {
	return kegGallonsMQTTTag;
}

void KegScaleObj::setKegPercentFullMQTTTag(String tag) {
	kegPercentFullMQTTTag = tag;
}

String KegScaleObj::getKegPercentFullMQTTTag() {
	return kegPercentFullMQTTTag;
}

void KegScaleObj::setKegGlassesLeftMQTTTag(String tag) {
	kegGlassesLeftMQTTTag = tag;
}

String KegScaleObj::getKegGlassesLeftMQTTTag() {
	return kegGlassesLeftMQTTTag;
}

void KegScaleObj::setScaleCalibrateEmptyMQTTTag(String tag) {
	scaleCalibrateEmptyMQTTTag = tag;
}

String KegScaleObj::getScaleCalibrateEmptyMQTTTag() {
	return scaleCalibrateEmptyMQTTTag;
}

void KegScaleObj::setScaleCalibrateFullMQTTTag(String tag) {
	scaleCalibrateFullMQTTTag = tag;
}

String KegScaleObj::getScaleCalibrateFullMQTTTag() {
	return scaleCalibrateFullMQTTTag;
}

void KegScaleObj::calibrateEmpty(int samples = 100, int retries = 10, unsigned long delay_ms = 100) {
	power_up();
	if (wait_ready_retry(retries, delay_ms)) {
		emptyCount = read_average(samples);
	}
	power_down();
}

void KegScaleObj::calibrateFull(int samples = 100, int retries = 10, unsigned long delay_ms = 100) {
	power_up();
	if (wait_ready_retry(retries, delay_ms)) {
		fullCount = read_average(samples);
	}
	power_down();
}

