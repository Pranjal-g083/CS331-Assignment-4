#include<stdio.h>
// liabraries for random number generation
#include<stdlib.h>
// liabraries for math functions
#include<math.h>
// liabraries for time functions
#include<time.h>
 #include <string.h>
#define MAX_POINTS 10000
#define MAX_DIMENSIONS 10000
#define MAX_COLS 100
#define MAX_ROWS 10000
// dimension of the points
int dimension;

// structure to store the points
struct Point
{
    double *x;
};

//************************* Function Declarations *************************//
// function to calculate the distance between two points

double Distance(struct Point p1,struct Point p2)
{
    double sum=0;
    for(int i=0;i<dimension;i++)
    {
        sum+=(p1.x[i]-p2.x[i])*(p1.x[i]-p2.x[i]);
    }
    return sum;
}
//***********************************************************************//

// function to calculate the centroid of the points
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
//***********************************************************************//

// function to calculate the variance of the points
double variance(struct Point* points,struct Point* centroids,int* clusters,int n)
{
    double sum=0;
    for(int i=0;i<n;i++)
    {
        sum+=Distance(points[i],centroids[clusters[i]]);
    }
    return sum/n;
}
//***********************************************************************//

// function to get the random k indices
int* get_random_k_points(int n,int k)
{
    int *random_k_points;
    random_k_points = (int*)malloc(k * sizeof(int));
    // initialize random seed
    for(int i = 0; i < k; i++)
    {
        int num = rand() % n;
        int flag=0;
        while(flag==0)
        {
            flag=1;
            for(int j=0;j<i;j++)
            {
                if(random_k_points[j]==num)
                {
                    flag=0;
                    break;
                }
            }
            if(flag==0)
            {
                num = rand() % n;
            }
        }
        random_k_points[i] = num;
    }
    return random_k_points;
}
//***********************************************************************//

// function to get the k means
int *KMeans(struct Point* points,int n,int k,int max_iter)
{
    int* best_cluster;      // array to store the best cluster
    //open file to write the output
    FILE *fp;
    fp = fopen("outputKMeansIntermediates.txt", "w");
    best_cluster=(int*)malloc(n*sizeof(int));   
    for(int i=0;i<n;i++)
    best_cluster[i]=0;
    struct Point* centroid_points;    // array to store the best centroids
    double best_variance;               // variable to store the best variance
    struct Point* centroid;
    centroid=(struct Point*)malloc(sizeof(struct Point));
    centroid[0]=Centroid(points,n);
    best_variance=variance(points,centroid,best_cluster,n);
    while(max_iter--)
    {
        srand(max_iter);        // initialize random seed
        int *random_k_points=get_random_k_points(n,k);      // get the random k indices
        for(int i=0;i<k;i++)
        {
            fprintf(fp,"starting point %d is (",i);         // print the starting points
            for(int j=0;j<dimension;j++)
            {
                fprintf(fp,"%f,",points[random_k_points[i]].x[j]);
            }
            fprintf(fp,")\n");
        }

        struct Point *centroids;                    // array to store the centroids
        centroids = (struct Point*)malloc(k * sizeof(struct Point));
        for(int i=0;i<k;i++)
        {
            centroids[i].x = (double*)malloc(dimension * sizeof(double));
            for(int j=0;j<dimension;j++)
            {
                centroids[i].x[j]=points[random_k_points[i]].x[j];    // initialize the centroids
            }
        }
        int *cluster;       // array to store the cluster
        cluster = (int*)malloc(n * sizeof(int));
        int flag=0;
        while(flag==0)      // repeat until convergence 
        {
            flag=1;
            for(int i=0;i<n;i++)    // assign the point to the closest cluster
            {
                double min=Distance(points[i],centroids[0]);
                int min_index=0;
                for(int j=1;j<k;j++)
                {
                    double temp=Distance(points[i],centroids[j]);
                    if(temp<min)
                    {
                        min=temp;
                        min_index=j;
                    }
                }
                if(cluster[i]!=min_index)
                {
                    flag=0;
                    cluster[i]=min_index;
                }
            }
            if(flag==0)         // if any point is reassigned, then recalculate the centroids
            {
                for(int i=0;i<k;i++)
                {
                    int count=0;
                    for(int j=0;j<dimension;j++)
                    {
                        centroids[i].x[j]=0;
                    }
                    for(int j=0;j<n;j++)
                    {
                        if(cluster[j]==i)
                        {
                            count++;
                            for(int l=0;l<dimension;l++)
                            {
                                centroids[i].x[l]+=points[j].x[l];
                            }
                        }
                    }
                    for(int j=0;j<dimension;j++)
                    {
                        centroids[i].x[j]/=count;
                    }
                }
            }
        }
        
        double variancei=variance(points,centroids,cluster,n);      // calculate the variance
        fprintf(fp," The variance for the above set of points is: %f\n",variancei);                                   // print the variance
        if(variancei<best_variance)                                 // if the variance is less than the best variance, then update the best variance and the best cluster
        {
            best_variance=variancei;
            best_cluster=cluster;
            centroid_points=centroids;
        }
    }
    fprintf(fp,"\n\n\n\n*****************************************************************************************\n");
    fprintf(fp,"***************************************************FINAL OUTPUT***************************\n");
    for(int i=0;i<k;i++)
    {
        fprintf(fp,"Centroid point %d is (",i);
        for(int j=0;j<dimension;j++)
        {
            fprintf(fp,"%f,",centroid_points[i].x[j]);
        }
        fprintf(fp,")\n");
    }
    fprintf(fp,"best variance is: %f\n",best_variance);             // print the best variance
    return best_cluster;
}
//***********************************************************************//


int num_points,  num_clusters;
double data[MAX_POINTS][MAX_DIMENSIONS];
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
    // close the CSV file
    fclose(csvfile);
    
    
    int n=rows;
    int k=4;
    printf("Please enter the number of clusters:");
    scanf("%d",&k);
    struct Point *points;
    points = (struct Point*)malloc(n * sizeof(struct Point));
    for(int i=0;i<n;i++)
    {
        points[i].x = (double*)malloc(dimension * sizeof(double));
        for(int j=0;j<dimension;j++)
        {
            points[i].x[j]=data[i][j];
        }
    }
    struct Point centroid=Centroid(points,n);
    printf("Centroid of points is (");
    for(int i=0;i<dimension;i++)
    {
        printf("%f,",centroid.x[i]);
    }
    printf(")\n");
    FILE *fp1=fopen("outputKMeansDescription.txt","w");
    int * clusters=KMeans(points,n,k,5);
    for(int i=0;i<n;i++)
    {
        fprintf(fp1,"point %d is (",i);
        for(int j=0;j<dimension;j++)
        {
            fprintf(fp1,"%f,",points[i].x[j]);
        }
        fprintf(fp1,")and it's cluster is %d\n", clusters[i]);
    }
    
    FILE *fp = fopen("outputKMeans.txt", "w"); // Create a file named "output.txt"
    if (fp == NULL) { // Check if the file was created successfully
        printf("Error creating file.\n");
        return 1;
    }
    
    for(int i = 0; i < n; i++) {
        fprintf(fp, "%d\n", clusters[i]); // Output each element of the array in a new line in the file
    }
    
    fclose(fp); // Close the file
    fclose(fp1);
    return 0;
}

//*******************************************************************************
//******************************END OF CODE**************************************
//*******************************************************************************