#include <iostream>
#include <fstream>
#include <sstream>

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <string.h>

#include <map>
#include <vector>

#include "extra.h"
#include "camera.h"

using namespace std;

namespace
{
	map<string, int> vmap;
	vector<Vector3f> vertices;
	vector<Vector3f> normals;
	vector<vector<int>> faces;
	vector<vector<Vector3f>> planes;
	int f_count = 0;
	int v_count = 0;


	void parseSTL(string source, string dest){
		string l;
		ifstream f(source);
		vector<int> face;
		//ofstream o("meshes/stand.off");
		if(f.is_open()){
			
			ofstream tmpf("meshes/tmp/tmpf.txt");
			ofstream tmpv("meshes/tmp/tmpv.txt");

			string tmp;
			//bool in_face = false;

			while(getline(f, l)){
				stringstream ss(l);
				ss >> tmp;
				if (tmp == "facet"){
					f_count +=1;
					ss >> tmp;
					Vector3f norm;
					ss >> norm[0] >> norm[1] >> norm[2];
					normals.push_back(norm);
					//in_face = true;
				}
				else if (tmp == "endfacet"){
					tmpf << "3 "+to_string(face[0])+" "+to_string(face[1])+" "+to_string(face[2])+"\n";
					vector<int> face_val = {face[0], face[1], face[2]};
					faces.push_back(face_val);
					face.clear();
				}
				else if (tmp == "vertex"){
					Vector3f vertex;
					ss >> vertex[0] >> vertex[1] >> vertex[2];
					string v = to_string(vertex[0]) + " "+to_string(vertex[1]) + " "+to_string(vertex[2]);
					map<std::basic_string<char>, int>::iterator loc = vmap.find(v);
					if (loc != vmap.end()){
						face.push_back(vmap[v]);
					}
					else{
						face.push_back(v_count);
						vmap[v] = v_count;
						vertices.push_back(vertex);
						v_count += 1;
						tmpv << v+ "\n";
					}
				}
				else continue;


			}

			tmpf.close();
			tmpv.close();

			ofstream o(dest);
			if (o.is_open()){
				int e_count = f_count+v_count+2;
				o << "OFF\n"+to_string(v_count) + " "+to_string(f_count) + " "+to_string(e_count) + "\n";
				ifstream fv("meshes/tmp/tmpv.txt");
				if (fv.is_open()){
					while(getline(fv, l)){
						o << l + "\n";
					}
					fv.close();
				}
				ifstream ff("meshes/tmp/tmpf.txt");
				if (ff.is_open()){
					while(getline(ff, l)){
						o << l + "\n";
					}
					ff.close();
				}


			o.close();
			}

			
			f.close();
		}
		else
		{
			cout << "Could not open file. Terminating program." << endl;
			exit(0);
		}


	}

	void parseOFF(string fname, string dest){

	}

	void drawMesh(){
		//const GLfloat yellow[4] = {.75,.75,.6,1.0};
		//glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
	    //glColorMaterial(GL_FRONT, GL_AMBIENT);
        glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_SMOOTH);

        //glColor3f(0.5f,0.5f,1.0f); 
		glBegin(GL_TRIANGLES);
		for(unsigned int i=0; i<faces.size(); i++){
			vector<int> face = faces[i];
			Vector3f norm = normals[i];
			for(unsigned int j=0; j<3; j++){
				Vector3f vertex = vertices[face[j]];
                if (i%4 == 0) {
                    glColor3f(1.0f,0.5f,0.5f);
                } else {
                    glColor3f(0.5f,0.5f,1.0f);
                }
				glNormal3d(norm.x(), norm.y(), norm.z());
				glVertex3d(vertex.x(), vertex.y(), vertex.z());
			}
		}
		glEnd();
	}

	void drawPlanes(){
		//const GLfloat green[4] = {.70,.85,.6,0.5};
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
		//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
        //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glEnable(GL_SMOOTH);
		glBegin(GL_TRIANGLES);

		glColor4f(.25,.75,.25,0.75);
		for(unsigned int i=0; i < planes.size(); i++){
			vector<Vector3f> plane = planes[i];
			Vector3f norm = -Vector3f::cross(plane[2]-plane[0], plane[1]-plane[0]);
			for(unsigned int j=0; j<3; j++){
				Vector3f vertex = plane[j];
				glNormal3d(norm.x(), norm.y(), norm.z());
				glVertex3d(vertex.x(), vertex.y(), vertex.z());
			}
            Vector3f vertex = plane[0];
            glNormal3d(norm.x(), norm.y(), norm.z());
            glVertex3d(vertex.x(), vertex.y(), vertex.z());
            vertex = plane[2];
            glNormal3d(norm.x(), norm.y(), norm.z());
            glVertex3d(vertex.x(), vertex.y(), vertex.z());
            vertex = plane[3];
            glNormal3d(norm.x(), norm.y(), norm.z());
            glVertex3d(vertex.x(), vertex.y(), vertex.z());
		} 		
		glEnd();
		
	}

	 // This is the camera
    Camera camera;
    
    

    // These are state variables for the UI
    bool g_mousePressed = false;

    // Declarations of functions whose implementations occur later.
    void arcballRotation(int endX, int endY);
    void keyboardFunc( unsigned char key, int x, int y);
    void specialFunc( int key, int x, int y );
    void mouseFunc(int button, int state, int x, int y);
    void motionFunc(int x, int y);
    void reshapeFunc(int w, int h);
    void drawScene(void);
    void initRendering();

    // This function is called whenever a "Normal" key press is
    // received.
    void keyboardFunc( unsigned char key, int x, int y )
    {
        switch ( key )
        {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
        {
            Matrix4f eye = Matrix4f::identity();
            camera.SetRotation( eye );
            camera.SetCenter( Vector3f::ZERO );
            break;
        }

        default:
            cout << "Unhandled key press " << key << "." << endl;        
        }

        glutPostRedisplay();
    }

    // This function is called whenever a "Special" key press is
    // received.  Right now, it's handling the arrow keys.
    void specialFunc( int key, int x, int y )
    {
        switch ( key )
        {

        }
        //glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {

    	int key = glutGetModifiers();

        if (state == GLUT_DOWN)
        {
            g_mousePressed = true;
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                //if (key == GLUT_ACTIVE_CTRL){
                	camera.MouseClick(Camera::LEFT, x, y);
                //}
          
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);

        camera.SetPerspective(50);
        glLoadMatrixf( camera.projectionMatrix() );
    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        glEnable(GL_NORMALIZE);

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

        // Clear to black
        glClearColor(0,0,0,1);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable( GL_BLEND );
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();              

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        GLfloat Lt0pos[] = {3.0,3.0,5.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        glLoadMatrixf( camera.viewMatrix() );

        // THIS IS WHERE THE DRAW CODE GOES.

        drawMesh();
        drawPlanes();

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if( g_mousePressed )
        {
            glPushMatrix();
            Vector3f eye = camera.GetCenter();
            glTranslatef( eye[0], eye[1], eye[2] );

            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3f(0,0,0); glVertex3f(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3f(0,0,0); glVertex3f(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3f(0,0,0); glVertex3f(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3f(0,0,0); glVertex3f(-1,0,0);
            glVertex3f(0,0,0); glVertex3f(0,-1,0);
            glVertex3f(0,0,0); glVertex3f(0,0,-1);

            glEnd();
            glPopMatrix();
            
            glPopAttrib();
            glPopMatrix();
        }
                 
        // Dump the image to the screen.
        glutSwapBuffers();
    }

}

int main (int argc, char* argv[]) {
	if (argc < 2){
		cout << "No file inputed. Program terminating." << endl;
		return 0;
	}
	string filef = argv[1];
	string ext = filef.substr(filef.find_last_of(".")+1);
	string filename = filef.substr(filef.find_last_of("/")+1);
	filename = "meshes/"+filename.substr(0, filename.length()-4)+"_model.off";

	for (int i = 0; i < ext.length(); i++){
		ext[i] = tolower(ext[i]);
	}

	if (ext == "stl"){
		parseSTL(filef, filename);
	}
	else if (ext == "off"){
		parseOFF(filef, filename);
	}




	vector<Vector3f> p;

	p.push_back(Vector3f(1.2, 3.0, 1.0));
	p.push_back(Vector3f(-1.2, 3.0, 1.0));
    p.push_back(Vector3f(-1.2,-3.0, 1.0));
	p.push_back(Vector3f(1.2, -3.0, 1.0));
	planes.push_back(p);


	//setup opengl
    glutInit( &argc, argv );

    // We're going to animate it, so double buffer 
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 600, 600 );
    
    camera.SetDimensions( 600, 600 );

    camera.SetDistance( 10 );
    camera.SetCenter( Vector3f::ZERO );

    glutCreateWindow("modeler");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

	return 0;
}