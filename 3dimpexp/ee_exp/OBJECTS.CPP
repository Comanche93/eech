// This was taken from EE sources

#ifndef NATIVE_TYPES
struct OBJECT_3D_BOUNDS { float xmin, xmax, ymin, ymax, zmin, zmax; };
struct OBJECT_3D
{
 int number_of_points, number_of_faces, number_of_surfaces;
#ifdef LOAD_NORMALS
 int number_of_point_normals;
#endif
#ifdef LOAD_RADIUS
 float radius;
#endif
 struct OBJECT_3D_BOUNDS bounding_box, bounding_box2;
 struct OBJECT_SHORT_3D_POINT { short int x, y, z; } *points;
 struct OBJECT_3D_FACE { unsigned char number_of_points; } *faces;
#ifdef LOAD_NORMALS
 struct OBJECT_3D_HEADING_PITCH_NORMAL { unsigned char heading, pitch; } *point_normals;
#endif
 struct POINT_3D_SHORT_REFERENCE { unsigned char point; } *object_faces_point_plain_list;
 struct FACE_SURFACE_DESCRIPTION
 {
  union
  {
   struct
   {
    unsigned short int polygons:1, detail:1, smoothed:1, textured:1, has_luminosity_texture:1, additive:1, translucent:1, luminous:1, has_specularity:1, texture_animation:1, texture_filtered:1, texture_mipmapped:1, texture_wrapped_u:1, texture_wrapped_v:1;
    unsigned char luminosity_texture_animation:1, luminosity_texture_filtered:1, luminosity_texture_mipmapped:1, luminosity_texture_wrapped_u:1, luminosity_texture_wrapped_v:1;
    unsigned char number_of_points;
   };
   unsigned int surface_flags;
  };
  unsigned short int texture_index, luminosity_texture_index, number_of_faces;
  union
  {
   struct { unsigned char reflectivity, specularity; };
   unsigned short int additional_surface_information;
  };
  union
  {
   struct { unsigned char blue, green, red, alpha; };
   unsigned int colour;
  };
 } *surfaces;
 struct OBJECT_3D_SHORT_TEXTURED_POINT { float u, v; } *surface_texture_points;
 struct POINT_3D_PLAIN_REFERENCE { unsigned short int point; } *surface_points;
#ifdef LOAD_NORMALS
 struct POINT_3D_PLAIN_REFERENCE /*{ unsigned short int point; }*/ *surface_point_normals;
#endif
};
typedef OBJECT_3D::OBJECT_SHORT_3D_POINT OBJECT_SHORT_3D_POINT;
typedef OBJECT_3D::OBJECT_3D_FACE OBJECT_3D_FACE;
typedef OBJECT_3D::POINT_3D_SHORT_REFERENCE POINT_3D_SHORT_REFERENCE;
typedef OBJECT_3D::FACE_SURFACE_DESCRIPTION FACE_SURFACE_DESCRIPTION;
typedef OBJECT_3D::OBJECT_3D_SHORT_TEXTURED_POINT OBJECT_3D_SHORT_TEXTURED_POINT;
typedef OBJECT_3D::POINT_3D_PLAIN_REFERENCE POINT_3D_PLAIN_REFERENCE;
#ifdef LOAD_NORMALS
typedef OBJECT_3D::OBJECT_3D_HEADING_PITCH_NORMAL OBJECT_3D_HEADING_PITCH_NORMAL;
#endif
#endif

class Objects
{
 private:

 void* loadfile(const char* filename)
 {
  FILE* file = safe_fopen(filename, "rb");
  fseek(file, 0, SEEK_END);
  unsigned size = ftell(file);
  void* data = malloc(size);
  fseek(file, 0, SEEK_SET);
  fread(data, size, 1, file);
  fclose(file);
  return data;
 }

 int total_number_of_objects;
 OBJECT_3D* objs;
 OBJECT_SHORT_3D_POINT* object_database_points;
 POINT_3D_PLAIN_REFERENCE* object_database_surface_point_list;
#ifdef LOAD_NORMALS
 POINT_3D_PLAIN_REFERENCE* object_database_surface_point_normal_list;
#endif
 OBJECT_3D_SHORT_TEXTURED_POINT* object_database_surface_point_texture_list;
 OBJECT_3D_FACE* object_database_faces;
#ifdef LOAD_NORMALS
 OBJECT_3D_HEADING_PITCH_NORMAL* object_database_point_normals;
#endif
 FACE_SURFACE_DESCRIPTION* object_database_surfaces;
 POINT_3D_SHORT_REFERENCE* object_database_faces_point_list;

 public:

 Objects(void)
 {
  int number_of_real_polygons;
  int number_of_surface_points, number_of_point_normals, number_of_surface_texture_points, number_of_point_reference, number_of_normal_reference;
#ifdef LOAD_NORMALS
  int number_of_surface_point_normals;
#endif
  int i, j;

  OBJECT_SHORT_3D_POINT* current_points;
  OBJECT_3D_FACE* current_faces;
#ifdef LOAD_NORMALS
  OBJECT_3D_HEADING_PITCH_NORMAL* current_point_normals;
#endif
  FACE_SURFACE_DESCRIPTION* current_surfaces;
  POINT_3D_SHORT_REFERENCE * current_faces_point_list;
  POINT_3D_PLAIN_REFERENCE * current_object_surface_point_references;
#ifdef LOAD_NORMALS
  POINT_3D_PLAIN_REFERENCE* current_object_surface_point_normal_references;
#endif
  OBJECT_3D_SHORT_TEXTURED_POINT * current_object_surface_point_texture_references;
  FILE* fp;

  fp = safe_fopen("3dobjs.bin", "rb");
  object_database_points = (OBJECT_SHORT_3D_POINT*)loadfile("3dobjs.pts");
  object_database_surface_point_list = (POINT_3D_PLAIN_REFERENCE*)loadfile("3dobjs.sp");
#ifdef LOAD_NORMALS
  object_database_surface_point_normal_list = (POINT_3D_PLAIN_REFERENCE*)loadfile("3dobjs.spn");
#endif
  object_database_surface_point_texture_list = (OBJECT_3D_SHORT_TEXTURED_POINT*)loadfile("3dobjs.spt");

  fread(&total_number_of_objects, sizeof(int), 1, fp);
  objs = (OBJECT_3D*)calloc(total_number_of_objects + 1, sizeof(OBJECT_3D));

  {
   int total_number_of_surfaces, total_number_of_plain_points, total_number_of_polygons;
#ifdef LOAD_NORMALS
   int total_number_of_point_normals;
#endif

   fread(&total_number_of_surfaces, sizeof(int), 1, fp);
#ifdef LOAD_NORMALS
   fseek(fp, sizeof(int), 1);
   fread(&total_number_of_point_normals, sizeof(int), 1, fp);
#else
   fseek(fp, 2 * sizeof(int), 1);
#endif
   fread(&total_number_of_plain_points, sizeof(int), 1, fp);
   fseek(fp, sizeof(int), 1);
   fread(&total_number_of_polygons, sizeof(int), 1, fp);

   object_database_faces = (OBJECT_3D_FACE *)calloc(sizeof(OBJECT_3D_FACE), total_number_of_polygons);
#ifdef LOAD_NORMALS
   object_database_point_normals = (OBJECT_3D_HEADING_PITCH_NORMAL *)calloc(sizeof(OBJECT_3D_HEADING_PITCH_NORMAL), total_number_of_point_normals);
#endif
   object_database_surfaces = (FACE_SURFACE_DESCRIPTION *)calloc(sizeof(FACE_SURFACE_DESCRIPTION), total_number_of_surfaces);
   object_database_faces_point_list = (POINT_3D_SHORT_REFERENCE *)calloc(sizeof(POINT_3D_SHORT_REFERENCE), total_number_of_plain_points);
  }

  current_points = object_database_points;
  current_faces = object_database_faces;
#ifdef LOAD_NORMALS
  current_point_normals = object_database_point_normals;
#endif
  current_surfaces = object_database_surfaces;
  current_faces_point_list = object_database_faces_point_list;
  current_object_surface_point_references = object_database_surface_point_list;
#ifdef LOAD_NORMALS
  current_object_surface_point_normal_references = object_database_surface_point_normal_list;
#endif
  current_object_surface_point_texture_references = object_database_surface_point_texture_list;

  for (i = 1; i <= total_number_of_objects; i++)
   {
    fread(&j, sizeof(int), 1, fp);

    if (j)
     continue;

    fread(&objs[i].number_of_points, sizeof(int), 1, fp);
    fread(&objs[i].number_of_surfaces, sizeof(int), 1, fp);
    fread(&objs[i].number_of_faces, sizeof(int), 1, fp);
    fread(&number_of_point_normals, sizeof(int), 1, fp);
#ifdef LOAD_NORMALS
    objs[i].number_of_point_normals = number_of_point_normals;
#endif
#ifdef LOAD_RADIUS
    fseek(fp, 2 * sizeof(int), SEEK_CUR);
    fread(&objs[i].radius, sizeof(float), 1, fp);
#else
    fseek(fp, 3 * sizeof(int), SEEK_CUR);
#endif
    fread(&number_of_surface_points, sizeof(int), 1, fp);
#ifdef LOAD_NORMALS
    fread(&number_of_surface_point_normals, sizeof(int), 1, fp);
#else
    fseek(fp, sizeof(int), SEEK_CUR);
#endif
    fread(&number_of_surface_texture_points, sizeof(int), 1, fp);
    fread(&objs[i].bounding_box, sizeof(OBJECT_3D_BOUNDS), 1, fp);
    fread(&objs[i].bounding_box2, sizeof(OBJECT_3D_BOUNDS), 1, fp);


    objs[i].surface_points = current_object_surface_point_references;
#ifdef LOAD_NORMALS
    objs[i].surface_point_normals = current_object_surface_point_normal_references;
#endif
    objs[i].surface_texture_points = current_object_surface_point_texture_references;
    current_object_surface_point_references += number_of_surface_points;
#ifdef LOAD_NORMALS
    current_object_surface_point_normal_references += number_of_surface_point_normals;
#endif
    current_object_surface_point_texture_references += number_of_surface_texture_points;
    objs[i].surfaces = current_surfaces;
    current_surfaces += objs[i].number_of_surfaces;

    number_of_real_polygons = 0;
    for (j = 0; j < objs[i].number_of_surfaces; j++)
     {
      fread(&objs[i].surfaces[j].surface_flags, sizeof(unsigned int), 1, fp);
      fread(&objs[i].surfaces[j].texture_index, sizeof(unsigned short int), 1, fp);
      fread(&objs[i].surfaces[j].luminosity_texture_index, sizeof(unsigned short int), 1, fp);
      fread(&objs[i].surfaces[j].number_of_faces, sizeof(int), 1, fp);
      fread(&objs[i].surfaces[j].colour, sizeof(unsigned int), 1, fp);
      fread(&objs[i].surfaces[j].reflectivity, sizeof(unsigned char), 1, fp);
      fread(&objs[i].surfaces[j].specularity, sizeof(unsigned char), 1, fp);

      if (objs[i].surfaces[j].polygons)
       number_of_real_polygons += objs[i].surfaces[j].number_of_faces;
     }

    objs[i].points = current_points;
    current_points += objs[i].number_of_points;
#ifdef LOAD_NORMALS
    objs[i].point_normals = current_point_normals;
    current_point_normals += number_of_point_normals;


    fread(objs[i].point_normals, 2, number_of_point_normals, fp);
#else
    fseek(fp, number_of_point_normals * 2, 1);
#endif

    fread(&number_of_point_reference, sizeof(int), 1, fp);
    fread(&number_of_normal_reference, sizeof(int), 1, fp);

    fread(current_faces, sizeof(OBJECT_3D_FACE), number_of_real_polygons, fp);
    objs[i].faces = current_faces;
    current_faces += number_of_real_polygons;

    fread(current_faces_point_list, sizeof(POINT_3D_SHORT_REFERENCE), number_of_point_reference, fp);
    objs[i].object_faces_point_plain_list = current_faces_point_list;
    current_faces_point_list += number_of_point_reference;
    fseek(fp, number_of_normal_reference * 2, 1);
   }

  fclose(fp);
 }

 ~Objects(void)
 {
  free(objs);
  free(object_database_points);
  free(object_database_surface_point_list);
#ifdef LOAD_NORMALS
  free(object_database_surface_point_normal_list);
#endif
  free(object_database_surface_point_texture_list);
  free(object_database_faces);
#ifdef LOAD_NORMALS
  free(object_database_point_normals);
#endif
  free(object_database_surfaces);
  free(object_database_faces_point_list);
 }

 int GetNumberOfObjects(void) const { return total_number_of_objects; }
 const OBJECT_3D& GetObject(int index) const { return objs[index]; }
 const OBJECT_3D& operator[](int index) const { return objs[index]; }
};
