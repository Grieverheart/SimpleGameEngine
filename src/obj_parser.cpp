#include <cstdlib>
#include <cstring>
#include <glm/glm.hpp>
#include "include/obj_parser.h"
#include "utility/fast_atof.h"

typedef unsigned short ushort;
typedef unsigned char uchar;

inline bool isSeparator( char token ){
	return (token == ' ' || 
			token == '\n' || 
			token == '\f' || 
			token == '\r' ||
			token == '\t' );
}

inline uchar getNextWord(const char* text, char* buff){
	uchar nchar = 0;
	while(!isSeparator(text[nchar])){
		buff[nchar] = text[nchar];
		++nchar;
	}
	buff[nchar] = '\0';
	return nchar + 1;
}

inline uchar findCharAndTerminate(char* text, char endchar){
	uchar i = 0;
	while(text[i] != endchar) ++i;
	text[i] = '\0';
	return i + 1;
}

std::vector<Vertex> parseObj(const char* filename, const std::string& shading_model){
	bool has_texture = false;
	FILE* file;
	char line[128];
	
	// Hold the data from the object file
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_vnormals;
	std::vector<glm::vec2> temp_vt;
	
	std::vector<ushort> normal_elements;
	std::vector<ushort> vertex_elements;
	std::vector<ushort> vt_elements;
	
	std::vector<Vertex> vertices;
	
	file = fopen(filename, "r");
	
	while( fgets(line, sizeof(line), file) != NULL){
		switch(line[0]){
		case 'v':
			switch(line[1]){
			case ' ':{
				uchar nchar = 2;
				glm::vec3 v;
				for(int i = 0; i < 3; i++){
					char buff[32];
					nchar += getNextWord(line + nchar, buff);
					v[i] = fast_atof(buff);
				}
				temp_vertices.push_back(v);
				break;
			}
			case 'n':{
				uchar nchar = 3;
				glm::vec3 vn;
				for(int i = 0; i < 3; i++){
					char buff[32];
					nchar += getNextWord(line + nchar, buff);
					vn[i] = fast_atof(buff);
				}
				temp_vnormals.push_back(vn);
				break;
			}
			case 't':{
				has_texture = true;
				uchar nchar = 3;
				glm::vec2 vt;
				for(int i = 0; i < 2; i++){
					char buff[32];
					nchar += getNextWord(line + nchar, buff);
					vt[i] = fast_atof(buff);
				}
				temp_vt.push_back(vt);
				break;
			}
			}
			break;
		case 'f':{
			ushort f,fn;
			if(!has_texture){
				uchar nchar = 2;
				for(int i = 0; i < 3; i++){
					char buff[32];
					nchar += getNextWord(line + nchar, buff);
					uchar nextpos = findCharAndTerminate(buff, '/');
					nextpos++;
					f = (ushort)strtoul10(buff);
					fn = (ushort)strtoul10(buff + nextpos);
					vertex_elements.push_back(--f);
					normal_elements.push_back(--fn);
				}
			}
			else{
				ushort vt;
				uchar nchar = 2;
				for(int i = 0; i < 3; i++){
					char buff[32];
					char *a = buff;
					nchar += getNextWord(line + nchar, buff);
					uchar nextpos = findCharAndTerminate(a, '/');
					f = (ushort)strtoul10(a);
					a += nextpos;
					nextpos = findCharAndTerminate(a, '/');
					vt = (ushort)strtoul10(a);
					a += nextpos;
					fn = (ushort)strtoul10(a);
					vertex_elements.push_back(--f);
					normal_elements.push_back(--fn);
					vt_elements.push_back(--vt);
				}
			}
			break;
		}
		default:
			break;
		}
	}
	
	fclose(file);
	// Make the vertex and normal lists
	int f_size = vertex_elements.size();
	
	if(shading_model == "smooth"){
		
		std::vector<glm::vec3> vertexnormals;
		int v_size = temp_vertices.size();
		
		for(int i = 0; i < v_size; i++){ //For every vertex
			glm::vec3 normal(0.0,0.0,0.0);
			for(int j = 0; j < f_size; j++){ //For every vertex of the triangles
				if(i == vertex_elements[j]){ //If they share the vertex
					normal+=temp_vnormals[normal_elements[j]];
				}
			}
			glm::normalize(normal);
			vertexnormals.push_back(normal);
		}
		if(!has_texture){
			for(int i = 0; i < f_size; i++){
				Vertex vertex(temp_vertices[vertex_elements[i]], vertexnormals[vertex_elements[i]]);
				vertices.push_back(vertex);
			}
		}
		else{
			for(int i = 0; i < f_size; i++){
				Vertex vertex(temp_vertices[vertex_elements[i]], vertexnormals[vertex_elements[i]], temp_vt[vt_elements[i]]);
				vertices.push_back(vertex);
			}
		}
	}
	else{
		if(!has_texture){
			for(int i = 0; i < f_size; i++){
				Vertex vertex(temp_vertices[vertex_elements[i]], temp_vnormals[normal_elements[i]]);
				vertices.push_back(vertex);
			}
		}
		else{
			for(int i = 0; i < f_size; i++){
				Vertex vertex(temp_vertices[vertex_elements[i]], temp_vnormals[normal_elements[i]], temp_vt[vt_elements[i]]);
				vertices.push_back(vertex);
			}
		}
	}
	return vertices;
}