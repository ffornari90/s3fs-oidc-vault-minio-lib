pipeline {
    environment {
        s3fsImage = "ffornari/s3fs-oidc-vault-minio"
        registryCredential = 'dockerhub'
        gitCredential = 'baltig'
        BUILD_VERSION = "latest"
    }
    
    agent any
    
    stages {
        stage('Initial update status on gitlab') {
            steps {
                echo 'Notify GitLab'
                updateGitlabCommitStatus name: 'build', state: 'pending'
            }
        }
        stage('Cloning git repo') {
            steps {
                script {
                    withCredentials([gitUsernamePassword(credentialsId: 'baltig')]) {
                        try {
                            sh "rm -rf s3fs-fuse-oidc-vault-minio-lib/; git clone https://baltig.infn.it/fornari/s3fs-fuse-oidc-vault-minio-lib.git"
                        } catch (e) {
                            updateGitlabCommitStatus name: 'clone', state: 'failed'
                        }
                    }
                }
            }
        }
        stage('Building docker images') {
            steps {
                script {
                    try {
                        sh "docker build -f s3fs-fuse-oidc-vault-minio-lib/docker/Dockerfile -t $s3fsImage:$BUILD_VERSION s3fs-fuse-oidc-vault-minio-lib/docker"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'build', state: 'failed'
                    }
                }
            }
        }
        stage('Logging into docker hub') {
            steps {
                script {
                    withCredentials([usernamePassword(credentialsId: "$registryCredential", passwordVariable: 'Password', usernameVariable: 'User')]) {
                        try {
                            sh "docker login -u ${env.User} -p ${env.Password}"
                        } catch (e) {
                            updateGitlabCommitStatus name: 'login', state: 'failed'
                        }
                    }
                }
            }
        }
        stage('Deploying docker images') {
            steps {
                script {
                    try {
                        sh "docker push $s3fsImage:$BUILD_VERSION"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'push', state: 'failed'
                    }
                }
            }
        }
        stage('Remove unused docker images') {
            steps {
                script {
                    try {
                        sh "docker rmi $s3fsImage:$BUILD_VERSION"
                    } catch (e) {
                        updateGitlabCommitStatus name: 'remove', state: 'failed'
                    }
                }
            }
        }
        stage('Final update status on gitlab') {
            steps {
                echo 'Notify GitLab'
                updateGitlabCommitStatus name: 'build', state: 'success'
            }
        }        
    }
}
