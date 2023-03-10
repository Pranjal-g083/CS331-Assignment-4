#include<stdio.h>
// liabraries for random number generation
#include<stdlib.h>
// liabraies for math functions
#include<math.h>
// liabraries for time
#include<time.h>
#include <string.h>
#define MAX_POINTS 10000
#define MAX_DIMENSIONS 10000
#define MAX_COLS 100
#define MAX_ROWS 10000
int cluster[MAX_ROWS]={0};
// dimension of the points
int dimension;
// epsilon is the value by which the centroids are shifted
double epsilon=0.01;

//*******************************************************************************

// point struct holding the coordinates of the point
struct Point
{
    double *x;
};

//*******************************************************************************

// function to calculate the euclidean distance between two points
double Distance(struct Point p1,struct Point p2)
{
    double sum=0;
    for(int i=0;i<dimension;i++)
    {
        sum+=(p1.x[i]-p2.x[i])*(p1.x[i]-p2.x[i]);
    }
    return sum;
}

//*******************************************************************************

// function to calculate the centroid of a set of points
struct Point Centroid(struct Point* points,int n)
{
    struct Point centroid;
    centroid.x=(double* )malloc(dimension*sizeof(double));
    for(int i = 0; i < n; i++)
    {
        for(int j=0;j<dimension;j++)
        {
            centroid.x[j] += points[i].x[j];
        }
    }
    for(int j=0;j<dimension;j++)
    {
        centroid.x[j] /= n;
    }
    return centroid;
}

//*******************************************************************************

// struct to hold the codebook
struct Codebook
{
    struct Point* centroids;
    int k;
};

//*******************************************************************************

// function to calculate the variance or distortion of the points
double distortion(struct Point* points,struct Codebook codebook,int* clusters,int n)
{
    double sum=0;
    for(int i=0;i<n;i++)
    {
        sum+=Distance(points[i],codebook.centroids[clusters[i]]);
    }
    return sum/n;
}


//*******************************************************************************

// function to assign the points to the nearest centroid
void Double_codebook(struct Codebook *codebook)
{

    codebook->k*=2;         // double the number of centroids
    codebook->centroids=(struct Point*)realloc(codebook->centroids,codebook->k*sizeof(struct Point));       // reallocate the memory for the centroids
    srand(time(0));     // seed the random number generator
    int j=rand()%dimension;     // choose a random dimension
    for(int i=0;i<codebook->k/2;i++)        // for each centroid in the first half of the codebook do the following
    {
        codebook->centroids[i+codebook->k/2].x=(double*)malloc(dimension*sizeof(double));
        for(int j=0;j<dimension;j++)
        {
            codebook->centroids[i+codebook->k/2].x[j]=codebook->centroids[i].x[j];
        }

        codebook->centroids[i+codebook->k/2].x[j]=codebook->centroids[i+codebook->k/2].x[j]+epsilon;    // shift the centroid by epsilon in the chosen dimension
        codebook->centroids[i].x[j]=codebook->centroids[i].x[j]-epsilon;    // shift the centroid by epsilon in the chosen dimension
    }

}

//*******************************************************************************

// function to cluster the points using the KMeans algorithm and return the cluster assignments and updating the codebook
int * KMeans(struct Codebook *codebook,struct Point* points,int n,int max_iter)
{
    int * best_clusters=(int*)malloc(n*sizeof(int));    // array to hold the best cluster assignments
    double best_distortion=1000000000;                  // variable to hold the best distortion
    for(int i=0;i<max_iter;i++)                            
    {
        // initialize the clusters
        int* clusters=(int*)malloc(n*sizeof(int));      // array to hold the cluster assignments
        for(int i=0;i<n;i++)
        {
            clusters[i]=0;
        }
        struct Codebook codebook_new;            // new codebook to hold the updated centroids  
        codebook_new.k=codebook->k;
        codebook_new.centroids=(struct Point*)malloc(codebook->k*sizeof(struct Point));
        for(int i=0;i<codebook->k;i++)
        {
            codebook_new.centroids[i]=codebook->centroids[i];
        }
        // assign clusters
        for(int i=0;i<n;i++)
        {
            double min_dist=Distance(points[i],codebook_new.centroids[0])+1;
            for(int j=0;j<codebook->k;j++)
            {
                double dist=Distance(points[i],codebook->centroids[j]);
                if(dist<min_dist)
                {
                    min_dist=dist;
                    clusters[i]=j;
                }
            }
        }
        /////////////////////////////////
        // update the centroids
        for(int i=0;i<n;i++){
            cluster[i] = clusters[i];
        }
        
        // update the centroids
        for(int i=0;i<codebook->k;i++)
        {
            int count=0;
            for(int j=0;j<n;j++)
            {
                if(clusters[j]==i)
                {
                    count++;
                }
            }
            struct Point* points_new=(struct Point*)malloc(count*sizeof(struct Point));
            int k=0;
            for(int j=0;j<n;j++)
            {
                if(clusters[j]==i)
                {
                    points_new[k]=points[j];
                    k++;
                }
            }
            codebook_new.centroids[i]=Centroid(points_new,count);
        }
        /////////////////////////////////
        // calculate the distortion
        double distortion_new=distortion(points,codebook_new,clusters,n);
        if(distortion_new<best_distortion)
        {
            best_distortion=distortion_new;
            for(int i=0;i<n;i++)
            {
                best_clusters[i]=clusters[i];
            }
            // update the codebook
            for(int i=0;i<codebook->k;i++)
            {
                codebook->centroids[i]=codebook_new.centroids[i];
            }
        }
    
    
    }
    printf("The best distortion is %f when codebook size is %d\n",best_distortion, codebook->k);    
    return best_clusters;
}

//*******************************************************************************
// function to cluster the points using the LBG algorithm and return the cluster assignments and updating the codebook
struct Codebook* LBGAlgo(struct Point* points,int n, int M, int max_iter)
{
    // initialize the codebook   
    struct Codebook *codebook;
    codebook=(struct Codebook*)malloc(sizeof(struct Codebook));
    codebook->k=1;
    codebook->centroids=(struct Point*)malloc(sizeof(struct Point));
    codebook->centroids[0]=Centroid(points,n);
    /////////////////////////////////

    // initialize the clusters
    int* clusters=(int*)malloc(n*sizeof(int));
    for(int i=0;i<n;i++)
    {
        clusters[i]=0;
    }
    /////////////////////////////////

    // print the initial codebook
    FILE *fp=fopen("outputLBGIntermediates.txt","w");
    fprintf(fp,"The number of centroids is %d\n",codebook->k);
    for(int i=0;i<codebook->k;i++)
        {
            fprintf(fp,"The centroid %d is (%f,%f)\n",i,codebook->centroids[i].x[0],codebook->centroids[i].x[1]);
        }
    /////////////////////////////////

    // initialize the distortion
    double distortion_old=distortion(points,*codebook,clusters,n);
    double distortion_new=0;
    // iterate until the codebook size is M

    while(codebook->k<M)
    {
        Double_codebook(codebook);  // double the codebook size
        clusters=KMeans(codebook,points,n,max_iter);    // cluster the points using KMeans
        distortion_new=distortion(points,*codebook,clusters,n); // calculate the distortion
        if(distortion_new>distortion_old)   // if the distortion increases, then stop
        {
            break;
        }
        else
        {
            distortion_old=distortion_new;
        }
        // print the codebook->after each iteration
        fprintf(fp,"The number of centroids is %d\n",codebook->k);
        for(int i=0;i<codebook->k;i++)
        {
            fprintf(fp,"The centroid %d is: (%f, ",i,codebook->centroids[i].x[0]);
            for(int j=1;j<dimension-1;j++)
            {
                fprintf(fp,"%f, ",i,codebook->centroids[i].x[j]);
            }
            fprintf(fp,"%f)\n",codebook->centroids[i].x[dimension-1]);
        }
    }
    fclose(fp);
    return codebook;
}

//*******************************************************************************

int main()
{
    
    float data[MAX_ROWS][MAX_COLS];
    int rows = 0, cols = 0;
    char buffer[1024];
    char *field;
    const char *delimiter = ",";  // assuming comma-separated values

    // open the CSV file for reading
    FILE *csvfile = fopen("data.csv", "r");

    if (csvfile == NULL) {
        printf("Could not open file\n");
        return 1;
    }

    // read the CSV file line by line
    while (fgets(buffer, 1024, csvfile)) {
        cols = 0;

        // split each line into fields using the delimiter
        field = strtok(buffer, delimiter);

        while (field != NULL && cols < MAX_COLS) {
            data[rows][cols++] = atof(field);  // convert string to float
            field = strtok(NULL, delimiter);
        }

        rows++;

        // check if we have exceeded the maximum number of rows
        if (rows == MAX_ROWS) {
            printf("Maximum number of rows exceeded\n");
            break;
        }
    }
    dimension=cols;
    fclose(csvfile);

    // initialize the points
    struct Point points[rows];
    for(int i=0;i<rows;i++)
    {
        points[i].x=(double*)malloc(dimension*sizeof(double));
    }
    for(int i=0;i<rows;i++)
    {
        for(int j=0;j<dimension;j++)
        {
            points[i].x[j]=data[i][j];
        }
    }
    // initialize the codebook
    int clusters=4;
    printf("Please enter the number of clusters:");
    scanf("%d",&clusters);
    struct Codebook* codebook=LBGAlgo(points,rows,clusters,200);
    FILE *fp1 = fopen("outputLBGDescription.txt", "w"); 
    for(int i=0;i<rows;i++)     // print the points and their cluster assignments
    {
        fprintf(fp1,"point %d is (",i);
        for(int j=0;j<dimension;j++)
        {
            fprintf(fp1,"%f,",points[i].x[j]);
        }
        fprintf(fp1,")and it's cluster is %d\n", cluster[i]);
    }
    
    FILE *fp = fopen("outputLBG.txt", "w"); 
    if (fp == NULL) { // Check if the file was created successfully
        printf("Error creating file.\n");
        return 1;
    }
    
    for(int i = 0; i < rows; i++) {
        fprintf(fp, "%d\n", cluster[i]); // Output each element of the array in a new line in the file
    }
    FILE *fp2 = fopen("outputLBGCodebook.txt", "w"); 
    for(int i=0;i<codebook->k;i++)  // print the codebook
    {
        fprintf(fp2,"centroid %d is (",i);
        for(int j=0;j<dimension;j++)
        {
            fprintf(fp2,"%f,",codebook->centroids[i].x[j]);
        }
        fprintf(fp2,")\n");
    }
    fclose(fp); // Close the file
    fclose(fp1); // Close the file
    fclose(fp2); // Close the file
    return 0;
}

//*******************************************************************************
//******************************END OF CODE**************************************
//*******************************************************************************