
#include <stdlib.h>
#include <iostream>
#include <string>
#include "VectorMath.h"
#include "Load_Image.h"
#include "GraphicsMath.h"

Image color_dark;
Image color_light;
Image normal_map;
Image highLight_map;
//assignment2
Image heightMap;
Image enviornment_map;
Image ReflectionMap;
Image highLight_color;

int width;
int height;
void createimage(bool spec, Image& output_image);
void createReflectiveImage(Image& output_image);

void assignment1();
void assignment2();

const char* outName = "../resources/output.png";
int image_type = 1;
int assignment = 1;

using namespace std;
int main(int args, char** argv) {

	if (assignment == 0) assignment1();
	else assignment2();

	return 0;
}




void assignment2() {

	color_dark.load("../resources/eye/DI0.png");
	color_light.load("../resources/eye/DI1.png");
	normal_map.load("../resources/eye/SM.png");

	heightMap.load("../resources/eye/DH.png");
	enviornment_map.load("../resources/eye/kool_dog.jpg");
	ReflectionMap.load("../resources/eye/DR.png");
	highLight_map.load("../resources/eye/DS.png");
	highLight_color.load("../resources/eye/DSC.png");


	//These are just to prove I can export images correctly
	color_light.write("../resources/OutGreen.png"); 
	normal_map.write("../resources/OutNormal.png");
	cout << "images done loading" << endl;

	width = normal_map.getWidth();
	height = normal_map.getHeight();
	Image output = Image(width, height);
	createReflectiveImage(output);
	output.write(outName);
	cout << "printed" << endl;
}


void createReflectiveImage(Image& output_image) {

	Image* dark_image;	//don't need these pointers can use direct reference.
	Image* light_image;
	dark_image = &color_dark;
	light_image = &color_light;
	vector3 l = vector3(6.0, 5.0, 5.0);
	l = l.normalize();
	vector3 eye = vector3(0.0, 0.0, 1.0);

	int pixel[3];
	int i, j;
	float cos;
	vector3 n;

	vector3 diffuse;
	vector3 specular;
	vector3 reflection;
	vector3 dark;
	vector3 light;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {

			//get the normal of each pixel. Based on that normal, do something.
			normal_map.getPixel(i, j, pixel);
			n.x = pixel[0];
			n.y = pixel[1];
			n.z = pixel[2];
			//convert the 8-bit range to a float from -1 to 1.
			n.x = ((n.x / 255.0f) * 2) - 1;
			n.y = ((n.y / 255.0f) * 2) - 1;
			n.z = ((n.z / 255.0f) * 2) - 1;



			//get cos of normal
			cos = vector3::dot(l, n);
			//clamp between 0 and 1
			float t = (0.5f * cos) + 0.5f;

			//******** calcuate diffuse *************
			//get image colors
			dark_image->getPixel(i, j, pixel);
			dark.x = pixel[0]; dark.y = pixel[1]; dark.z = pixel[2];
			light_image->getPixel(i, j, pixel);
			light.x = pixel[0]; light.y = pixel[1]; light.z = pixel[2];

			//set diffuse to be between the two images.
			diffuse.x = (dark.x * (1.0f - t) + light.x * t);
			diffuse.y = (dark.y * (1.0f - t) + light.y * t);
			diffuse.z = (dark.z * (1.0f - t) + (light.z * t));


			//******** calcuate specular *************
			float S = 0.0;
			S = -l.z + 2.0f * (l.x * n.x + l.y * n.y + l.z * n.z) * n.z;
			//the eye and the light vector are the same in this instance
			if (S < 0) S = 0; if (S > 1) S = 1;
			highLight_color.getPixel(i, j, pixel);
			specular.x = pixel[0]; specular.y = pixel[1]; specular.z = pixel[2];
			highLight_map.getPixel(i, j, pixel);
			float S_mapping = pixel[0] / 255.0f;

			//********* calculate reflection ************
			
			float h = 0.0f;
			float d = 1.1f;
			vector3 r = reflection_angle(eye, n);
			vector3 reflectionPos;
			reflectionPos.x = ((d - h) * (r.x / r.z)) + i;
			reflectionPos.y = ((d - h) * (r.y / r.z)) + j;
			//reflectionPos.x = (2 * r.x * r.z * d) / (2 * r.z * r.z - 1);
			//reflectionPos.y = (2 * r.y * r.z * d) / (2 * r.z * r.z - 1);
			reflectionPos.z = d;
			if ((int)reflectionPos.x < 0) {
				reflectionPos.x = abs( (int)reflectionPos.x % 1260) ;
				//reflectionPos.x = 0;
			}
			if ((int)reflectionPos.y < 0) {
				reflectionPos.y = abs((int)reflectionPos.x % 1679);
				//reflectionPos.y = 0;
			}
			enviornment_map.getPixel((int)reflectionPos.x, (int)reflectionPos.y, pixel);
			reflection.x = pixel[0]; reflection.y = pixel[1]; reflection.z = pixel[2];

			ReflectionMap.getPixel(i, j, pixel);
			float R_mapping = pixel[0] / 255.0f;
			//R_mapping = 0.0;
			//combine
			specular = specular * (S * S_mapping);
			diffuse = diffuse * (1.0f - (S * S_mapping));
			vector3 finalColor = specular + diffuse;
			reflection = reflection * R_mapping;
			finalColor = finalColor * (1.0f - R_mapping);
			finalColor = finalColor + reflection;


			pixel[0] = finalColor.x; pixel[1] = finalColor.y; pixel[2] = finalColor.z;
			output_image.setPixel(i, j, pixel);
		}
	}

	//final_image = normal_map; //DELETE

}







/*
void createReflectiveImage( Image& output_image) {

	//Image final_image();

	Image* dark_image;	//don't need these pointers can use direct reference.
	Image* light_image;
	dark_image = &color_dark;
	light_image = &color_light;
	vector3 l = vector3(0.0, 0.0, -1.0);
	l = l.normalize();
	vector3 n; //normal angle
	vector3 r; //reflection angle

	float d = 1.1f; //reflectionmapDistance

	int pixel[4];
	int i, j;
	float cos;

	vector3 diffuse; //these are the colors of the different elements of our scene
	vector3 specular;
	vector3 dark;
	vector3 light;
	vector3 reflect;

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {


			//********** PART 1: get Normal of surface (and maybe other things like height) **************

			//get the normal of each pixel. Based on that normal, do something.
			normal_map.getPixel(i, j, pixel);
			n.x = pixel[0];
			n.y = pixel[1];
			n.z = pixel[2];
			//convert the 8-bit range to a float from -1 to 1.
			n.x = ((n.x / 255.0f) * 2) - 1;
			n.y = ((n.y / 255.0f) * 2) - 1;
			n.z = ((n.z / 255.0f) * 2) - 1;


			//get cos of normal
			cos = vector3::dot(l, n);
			//clamp between 0 and 1
			float t = (0.5f * cos) + 0.5f;

			//****************** PART 2: get intensity of each color **************************

			highLight_map.getPixel(i, j, pixel);
			float S = ((float)pixel[0] / 255.0f); //specular intensity

			ReflectionMap.getPixel(i, j, pixel);
			float R = ((float)pixel[0] / 255.0f); //reflective intensity



			float D = 1.0f; //diffuse intensity

			//********************* PART 3: calcuate diffuse *************************

			//get image colors
			dark_image->getPixel(i, j, pixel);
			dark.x = pixel[0]; dark.y = pixel[1]; dark.z = pixel[2];
			light_image->getPixel(i, j, pixel);
			light.x = pixel[0]; light.y = pixel[1]; light.z = pixel[2];

			//set diffuse to be between the two images.
			diffuse.x = (dark.x * (1.0f - t) + light.x * t);
			diffuse.y = (dark.y * (1.0f - t) + light.y * t);
			diffuse.z = (dark.z * (1.0f - t) + (light.z * t));


			//********************* PART 4: calcuate specular *************************
			
			float S_angle = -l.z + 2.0f * (l.x * n.x + l.y * n.y + l.z * n.z) * n.z;
			//the eye and the light vector are the same in this instance - or are they?
			if (S_angle < 0) S_angle = 0; if (S_angle > 1) S_angle = 1;
			highLight_map.getPixel(i, j, pixel);
			specular.x = pixel[0]; specular.y = pixel[1]; specular.z = pixel[2];

			//********************* PART 5: calcuate reflection *************************
			//heightMap.getPixel(i, j, pixel);
			//float h = (float)pixel[0];
			//float h = 0.0f;
			//r = reflection_angle(l, n);
			//if (r.z > 0.0) {
			//	vector3 reflectionPos;
			//	reflectionPos.x = (d - h) * (r.x / r.z) + i;
			//	reflectionPos.y = (d - h) * (r.y / r.z) + j;
			//	reflectionPos.z = d; //just added in for help, not used
			//	float u = (reflectionPos.x-0)/(1260-reflectionPos.x);
			//	clamp(u);
			//	float v = (reflectionPos.y - 0) / (1679 - reflectionPos.y);
			//	clamp(v);

			//	enviornment_map.getPixel(u, v, pixel);
			//	reflect.x = pixel[0]; reflect.y = pixel[1]; reflect.z = pixel[2];
			//}


			//Combine elements

			//combine specular and diffuse
			specular = specular * S_angle * S;
			diffuse = diffuse * (1.0f - S_angle * S) ;
			vector3 finalColor = specular + diffuse;

			//reflect = reflect * R;
			//finalColor = finalColor * 1.0 - R;
			//finalColor = finalColor + reflect;

			pixel[0] = finalColor.x; pixel[1] = finalColor.y; pixel[2] = finalColor.z;
			output_image.setPixel(i, j, pixel);
		}
	}
	cout << "image created" << endl;
	//final_image = normal_map; //DELETE
	//return output_image;

}
*/

//****************previous assignment1**************************//




























/**************************************Assignment 1*****************************************/
void assignment1() {

	if (image_type > 1) {
		color_dark.load("../resources/scene_map/dark.png");
		color_light.load("../resources/scene_map/light.png");
		normal_map.load("../resources/scene_map/normal.png");
		highLight_map.load("../resources/scene_map/highlight.png");
	}
	else {
		color_dark.load("../resources/color_map/dark.png");
		color_light.load("../resources/color_map/light.png");
		normal_map.load("../resources/color_map/normal.png");
		highLight_map.load("../resources/color_map/highlight.png");
	}

	color_dark.load("../resources/test_map/dark.png");
	color_light.load("../resources/test_map/light.png");
	normal_map.load("../resources/test_map/normal.png");
	highLight_map.load("../resources/test_map/highlight.png");


	color_light.write("../resources/OutGreen.png"); //These are just to prove I can export images correctly
	normal_map.write("../resources/OutNormal.png");
	cout << "images done loading" << endl;

	width = normal_map.getWidth();
	height = normal_map.getHeight();

	Image output = Image(width, height);
	switch (image_type) {
	case 0:
		//solid color without specular
		createimage(false, output);
		break;
	case 1:
		//solid color with specular
		createimage(true, output);
		break;
	case 2:
		//picture without specular
		createimage(false, output);
		break;
	case 3:
		//picture with specular
		createimage(true, output);
		break;
	}

	output.write(outName);
	cout << "printed" << endl;
}



void createimage(bool spec, Image &output_image) {

		//Image final_image();

		Image* dark_image;	//don't need these pointers can use direct reference.
		Image* light_image;
		dark_image = &color_dark;
		light_image = &color_light;
		vector3 l = vector3(0.0, 0.0, 1.0);
		


		int pixel[3];
		int i, j;
		float cos;
		vector3 n;

		vector3 diffuse;
		vector3 specular;
		vector3 dark;
		vector3 light;

		for (j = 0; j < height; j++) {
			for (i = 0; i < width; i++) {
				
				//get the normal of each pixel. Based on that normal, do something.
				normal_map.getPixel(i, j, pixel);
				n.x = pixel[0];
				n.y = pixel[1];
				n.z = pixel[2];
				//convert the 8-bit range to a float from -1 to 1.
				n.x = ( (n.x / 255.0f) * 2 )- 1;
				n.y = ( (n.y / 255.0f) * 2) - 1;
				n.z = ( (n.z / 255.0f) * 2) - 1;
				

				//get cos of normal
				cos = vector3::dot(l, n);
				//clamp between 0 and 1
 				float t = (0.5f * cos) + 0.5f; 

				//******** calcuate diffuse *************//
				//get image colors
				dark_image->getPixel(i, j, pixel);
				dark.x = pixel[0]; dark.y = pixel[1]; dark.z = pixel[2];
				light_image->getPixel(i, j, pixel);
				light.x = pixel[0]; light.y = pixel[1]; light.z = pixel[2];

				//set diffuse to be between the two images.
				diffuse.x = (dark.x * (1.0f - t) + light.x * t);
				diffuse.y = (dark.y * (1.0f - t) + light.y * t);
				diffuse.z = ( dark.z * (1.0f - t) + (light.z * t) );
				//2. calculate percentage of specular if applicable
				float S = 0.0;
				if (spec) {
					S = -l.z + 2.0f * (l.x * n.x + l.y * n.y + l.z * n.z) * n.z; 
					//the eye and the light vector are the same in this instance
					if (S < 0) S = 0; if (S > 1) S = 1;
				}
				highLight_map.getPixel(i, j, pixel);
				specular.x = pixel[0]; specular.y = pixel[1]; specular.z = pixel[2];

				specular = specular * S;
				diffuse = diffuse * (1.0f - S);
				vector3 finalColor = specular + diffuse;
				pixel[0] = finalColor.x; pixel[1] = finalColor.y; pixel[2] = finalColor.z;
				output_image.setPixel(i, j, pixel);
			}
	}
		cout << "image created" << endl;
		//final_image = normal_map; //DELETE
		//return output_image;

}
